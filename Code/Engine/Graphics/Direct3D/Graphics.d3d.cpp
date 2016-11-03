// Header Files
//=============

#include "../Graphics.h"

#include <cstddef>
#include <cstdint>
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX11core.h>
#include <DXGI.h>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Time/Time.h"
#include "../CommonData.h"
#include "../ConstantBuffer.h"
#include "../../Math/cMatrix_transformation.h"
// Static Data Initialization
//===========================

namespace
{
	using namespace eae6320::Graphics;
	CommonData *commonData = CommonData::GetCommonData();
	std::vector < eae6320::Gameplay::GameObject* > gameObjects;
	// This is the main window handle from Windows
	HWND s_renderingWindow = NULL;
	// These are D3D interfaces
	IDXGISwapChain* s_swapChain = NULL;
	ID3D11RenderTargetView* s_renderTargetView = NULL;
	ID3D11DepthStencilView* s_depthStencilView = NULL;

	// The vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want
	ID3D11VertexShader* s_vertexShader = NULL;
	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	ID3D11PixelShader* s_fragmentShader = NULL;

	// This struct determines the layout of the constant data that the CPU will send to the GPU

	ConstantBufferData::sFrame frameBufferData;
	ConstantBuffer frameBuffer;
	ConstantBuffer drawCallBuffer;

	eae6320::Camera::cCamera* camera;
}
// Helper Function Declarations
//=============================

namespace
{
	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool CreateViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool LoadFragmentShader();
	bool LoadVertexShader(eae6320::Platform::sDataFromFile* o_compiledShader);
}

// Interface
//==========

// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		commonData->s_direct3dImmediateContext->ClearRenderTargetView(s_renderTargetView, clearColor);
	}

	{
		const float depthToClear = 1.0f;
		const uint8_t stencilToClear = 0; // Arbitrary until stencil is used
		commonData->s_direct3dImmediateContext->ClearDepthStencilView(s_depthStencilView, D3D11_CLEAR_DEPTH,
			depthToClear, stencilToClear);
	}
	// Update the constant buffer
	frameBufferData.g_transform_worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(camera->GetOrientation(), camera->GetPosition());
	frameBufferData.g_transform_cameraToScreen = Math::cMatrix_transformation::CreateCameraToScreenTransform_perspectiveProjection(camera->GetFieldOfView(), camera->GetAspectRatio(), camera->GetNearPlaneDistance(), camera->GetFarPlaneDistance());
	frameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	frameBuffer.UpdateConstantBuffer(&frameBufferData, sizeof(frameBufferData));

	// Draw the geometry
	{
		// Set the vertex and fragment shaders
		{
			ID3D11ClassInstance** const noInterfaces = NULL;
			const unsigned int interfaceCount = 0;
			commonData->s_direct3dImmediateContext->VSSetShader(s_vertexShader, noInterfaces, interfaceCount);
			commonData->s_direct3dImmediateContext->PSSetShader(s_fragmentShader, noInterfaces, interfaceCount);
		}

		size_t numberOfMeshes = gameObjects.size();
		ConstantBufferData::sDrawCall drawCallBufferData;
		for (size_t i = 0; i < numberOfMeshes; i++)
		{
			drawCallBufferData.g_transform_localToWorld = Math::cMatrix_transformation(gameObjects[i]->GetOrientation(), gameObjects[i]->GetPosition());
			drawCallBuffer.UpdateConstantBuffer(&drawCallBufferData, sizeof(drawCallBufferData));
			gameObjects[i]->GetMesh()->RenderMesh();
		}
		gameObjects._Pop_back_n(numberOfMeshes);
		gameObjects.clear();
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (to the front buffer)
	{
		const unsigned int swapImmediately = 0;
		const unsigned int presentNextFrame = 0;
		const HRESULT result = s_swapChain->Present(swapImmediately, presentNextFrame);
		EAE6320_ASSERT(SUCCEEDED(result));
	}
}

void eae6320::Graphics::SetCamera(Camera::cCamera * Camera)
{
	camera = Camera;
}

// Initialization / Clean Up
//--------------------------

bool eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	bool wereThereErrors = false;

	s_renderingWindow = i_initializationParameters.mainWindow;

	// Create an interface to a Direct3D device
	if (!CreateDevice(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	// Initialize the viewport of the device
	if (!CreateViews(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight))
	{
		wereThereErrors = true;
		goto OnExit;
	}

	// Initialize the graphics objects
	if (!LoadVertexShader(commonData->compiledVertexShader))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	if (!LoadFragmentShader())
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if (!frameBuffer.CreateConstantBuffer(ConstantBufferType::FRAME, sizeof(ConstantBufferData::sFrame), &frameBufferData))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	else
	{
		frameBuffer.BindingConstantBuffer(BindMode::VS_PS_BOTH);
	}

	if (!drawCallBuffer.CreateConstantBuffer(ConstantBufferType::DRAWCALL, sizeof(ConstantBufferData::sDrawCall)))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	else
	{
		drawCallBuffer.BindingConstantBuffer(BindMode::VS_ONLY);
	}

OnExit:

	// A vertex shader object is used to render the triangle.
	// The compiled vertex shader is the actual compiled code,
	// and once it has been used to create the vertex input layout
	// it can be freed.
	if (commonData->compiledVertexShader && wereThereErrors)
	{
		commonData->compiledVertexShader->Free();
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if (commonData->s_direct3dDevice)
	{
		if (commonData->s_vertexLayout)
		{
			commonData->s_vertexLayout->Release();
			commonData->s_vertexLayout = NULL;
		}

		if (s_vertexShader)
		{
			s_vertexShader->Release();
			s_vertexShader = NULL;
		}

		if (s_fragmentShader)
		{
			s_fragmentShader->Release();
			s_fragmentShader = NULL;
		}

		if (!frameBuffer.CleanUpConstantBuffer())
		{

		}
		if (!drawCallBuffer.CleanUpConstantBuffer())
		{

		}
		if (s_renderTargetView)
		{
			s_renderTargetView->Release();
			s_renderTargetView = NULL;
		}
		if (s_depthStencilView)
		{
			s_depthStencilView->Release();
			s_depthStencilView = NULL;
		}

		commonData->s_direct3dDevice->Release();
		commonData->s_direct3dDevice = NULL;
	}
	if (commonData->s_direct3dImmediateContext)
	{
		commonData->s_direct3dImmediateContext->Release();
		commonData->s_direct3dImmediateContext = NULL;
	}
	if (s_swapChain)
	{
		s_swapChain->Release();
		s_swapChain = NULL;
	}

	s_renderingWindow = NULL;
	return !wereThereErrors;
}

void eae6320::Graphics::SetGameObjectData(Gameplay::GameObject*gameObject, const Math::cVector startPosition, const Math::cVector startOrientation)
{
	if (gameObject)
	{
		gameObjects.push_back(gameObject);
		gameObject->SetNewInitialPositionOffset(startPosition);
		gameObject->SetNewInitialEularOffset(startOrientation);
	}
	else
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Trying to draw a non existent gameobject. Check gameobject name");
	}
}

// Helper Function Definitions
//============================

namespace
{
	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		IDXGIAdapter* const useDefaultAdapter = NULL;
		const D3D_DRIVER_TYPE useHardwareRendering = D3D_DRIVER_TYPE_HARDWARE;
		const HMODULE dontUseSoftwareRendering = NULL;
		unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		{
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		}
		D3D_FEATURE_LEVEL* const useDefaultFeatureLevels = NULL;
		const unsigned int requestedFeatureLevelCount = 0;
		const unsigned int sdkVersion = D3D11_SDK_VERSION;
		DXGI_SWAP_CHAIN_DESC swapChainDescription = { 0 };
		{
			{
				DXGI_MODE_DESC& bufferDescription = swapChainDescription.BufferDesc;

				bufferDescription.Width = i_resolutionWidth;
				bufferDescription.Height = i_resolutionHeight;
				{
					DXGI_RATIONAL& refreshRate = bufferDescription.RefreshRate;

					refreshRate.Numerator = 0;	// Refresh as fast as possible
					refreshRate.Denominator = 1;
				}
				bufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				bufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			}
			{
				DXGI_SAMPLE_DESC& multiSamplingDescription = swapChainDescription.SampleDesc;

				multiSamplingDescription.Count = 1;
				multiSamplingDescription.Quality = 0;	// Anti-aliasing is disabled
			}
			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescription.BufferCount = 1;
			swapChainDescription.OutputWindow = s_renderingWindow;
			swapChainDescription.Windowed = TRUE;
			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDescription.Flags = 0;
		}
		D3D_FEATURE_LEVEL highestSupportedFeatureLevel;
		const HRESULT result = D3D11CreateDeviceAndSwapChain(useDefaultAdapter, useHardwareRendering, dontUseSoftwareRendering,
			flags, useDefaultFeatureLevels, requestedFeatureLevelCount, sdkVersion, &swapChainDescription,
			&s_swapChain, &commonData->s_direct3dDevice, &highestSupportedFeatureLevel, &commonData->s_direct3dImmediateContext);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a Direct3D11 device with HRESULT %#010x", result);
			return false;
		}
	}

	/*bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		bool wereThereErrors = false;

		// Create and bind the render target view
		ID3D11Texture2D* backBuffer = NULL;
		{
			// Get the back buffer from the swap chain
			HRESULT result;
			{
				const unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
				result = s_swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = NULL;
				result = commonData->s_direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &s_renderTargetView);
			}
			if (SUCCEEDED(result))
			{
				// Bind it
				const unsigned int renderTargetCount = 1;
				ID3D11DepthStencilView* const noDepthStencilState = NULL;
				commonData->s_direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &s_renderTargetView, noDepthStencilState);
			}
			else
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the render target view with HRESULT %#010x", result);
				goto OnExit;
			}
		}

		// Specify that the entire render target should be visible
		{
			D3D11_VIEWPORT viewPort = { 0 };
			viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
			viewPort.Width = static_cast<float>(i_resolutionWidth);
			viewPort.Height = static_cast<float>(i_resolutionHeight);
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			const unsigned int viewPortCount = 1;
			commonData->s_direct3dImmediateContext->RSSetViewports(viewPortCount, &viewPort);
		}

	OnExit:

		if (backBuffer)
		{
			backBuffer->Release();
			backBuffer = NULL;
		}

		return !wereThereErrors;
	}*/

	bool CreateViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		bool wereThereErrors = false;

		ID3D11Texture2D* backBuffer = NULL;
		ID3D11Texture2D* depthBuffer = NULL;

		// Create a "render target view" of the back buffer
		// (the back buffer was already created by the call to D3D11CreateDeviceAndSwapChain(),
		// but we need a "view" of it to use as a "render target",
		// meaning a texture that the GPU can render to)
		{
			// Get the back buffer from the swap chain
			{
				const unsigned int bufferIndex = 0; // This must be 0 since the swap chain is discarded
				const HRESULT result = s_swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = NULL;
				const HRESULT result = commonData->s_direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &s_renderTargetView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the render target view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}
		// Create a depth/stencil buffer and a view of it
		{
			// Unlike the back buffer no depth/stencil buffer exists until and unless we create it
			{
				D3D11_TEXTURE2D_DESC textureDescription = { 0 };
				{
					textureDescription.Width = i_resolutionWidth;
					textureDescription.Height = i_resolutionHeight;
					textureDescription.MipLevels = 1; // A depth buffer has no MIP maps
					textureDescription.ArraySize = 1;
					textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
					{
						DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
						sampleDescription.Count = 1; // No multisampling
						sampleDescription.Quality = 0; // Doesn't matter when Count is 1
					}
					textureDescription.Usage = D3D11_USAGE_DEFAULT; // Allows the GPU to write to it
					textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					textureDescription.CPUAccessFlags = 0; // CPU doesn't need access
					textureDescription.MiscFlags = 0;
				}
				// The GPU renders to the depth/stencil buffer and so there is no initial data
				// (like there would be with a traditional texture loaded from disk)
				const D3D11_SUBRESOURCE_DATA* const noInitialData = NULL;
				const HRESULT result = commonData->s_direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = NULL;
				const HRESULT result = commonData->s_direct3dDevice->CreateDepthStencilView(depthBuffer, noSubResources, &s_depthStencilView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}

		// Bind the views
		{
			const unsigned int renderTargetCount = 1;
			commonData->s_direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &s_renderTargetView, s_depthStencilView);
		}
		// Specify that the entire render target should be visible
		{
			D3D11_VIEWPORT viewPort = { 0 };
			viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
			viewPort.Width = static_cast<float>(i_resolutionWidth);
			viewPort.Height = static_cast<float>(i_resolutionHeight);
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			const unsigned int viewPortCount = 1;
			commonData->s_direct3dImmediateContext->RSSetViewports(viewPortCount, &viewPort);
		}

	OnExit:

		if (backBuffer)
		{
			backBuffer->Release();
			backBuffer = NULL;
		}
		if (depthBuffer)
		{
			depthBuffer->Release();
			depthBuffer = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadFragmentShader()
		/*{
			// Load the source code and compile it
			ID3D10Blob* compiledShader = NULL;
			{
				const char* const sourceCodeFileName = "data/Shaders/fragmentShader.hlsl";
				D3D10_SHADER_MACRO* const noMacros = NULL;
				ID3DInclude* const noIncludes = NULL;
				const char* const entryPoint = "main";
				const char* const profile = "ps_4_0";
				const unsigned int noFlags = 0;
				ID3DX11ThreadPump* const blockUntilLoaded = NULL;
				ID3D10Blob* errorMessages = NULL;
				HRESULT result;
				result = D3DX11CompileFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile,
					noFlags, noFlags, blockUntilLoaded, &compiledShader, &errorMessages, &result);
				if (SUCCEEDED(result))
				{
					if (errorMessages)
					{
						errorMessages->Release();
					}
				}
				else
				{
					if (errorMessages)
					{
						EAE6320_ASSERTF(false, reinterpret_cast<char*>(errorMessages->GetBufferPointer()));
						eae6320::Logging::OutputError("Direct3D failed to compile the fragment shader from the file %s: %s",
							sourceCodeFileName, reinterpret_cast<char*>(errorMessages->GetBufferPointer()));
						errorMessages->Release();
					}
					else
					{
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Direct3D failed to compile the fragment shader from the file %s",
							sourceCodeFileName);
					}
					return false;
				}
			}
			// Create the fragment shader object
			bool wereThereErrors = false;
			{
				ID3D11ClassLinkage* const noInterfaces = NULL;
				const HRESULT result = commonData->s_direct3dDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
					noInterfaces, &s_fragmentShader);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the fragment shader with HRESULT %#010x", result);
					wereThereErrors = true;
				}
				compiledShader->Release();
			}
			return !wereThereErrors;
		}*/
	{
		bool wereThereErrors = false;

		// Load the compiled shader
		eae6320::Platform::sDataFromFile compiledShader;
		const char* const path = "data/shaders/fragment.binshader";
		{
			std::string errorMessage;
			if (!eae6320::Platform::LoadBinaryFile(path, compiledShader, &errorMessage))
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", path, errorMessage.c_str());
				goto OnExit;
			}
		}
		// Create the shader object
		{
			ID3D11ClassLinkage* const noInterfaces = NULL;
			const HRESULT result = commonData->s_direct3dDevice->CreatePixelShader(
				compiledShader.data, compiledShader.size, noInterfaces, &s_fragmentShader);
			if (FAILED(result))
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", path, result);
				goto OnExit;
			}
		}

	OnExit:

		compiledShader.Free();

		return !wereThereErrors;
	}

	bool LoadVertexShader(eae6320::Platform::sDataFromFile* o_compiledShader)
		/*{
			// Load the source code and compile it
			{
				const char* const sourceCodeFileName = "data/Shaders/vertexShader.hlsl";
				D3D10_SHADER_MACRO* const noMacros = NULL;
				ID3DInclude* const noIncludes = NULL;
				const char* const entryPoint = "main";
				const char* const profile = "vs_4_0";
				const unsigned int noFlags = 0;
				ID3DX11ThreadPump* const blockUntilLoaded = NULL;
				ID3D10Blob* errorMessages = NULL;
				HRESULT result;
				result = D3DX11CompileFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile,
					noFlags, noFlags, blockUntilLoaded, &o_compiledShader, &errorMessages, &result);
				if (SUCCEEDED(result))
				{
					if (errorMessages)
					{
						errorMessages->Release();
					}
				}
				else
				{
					if (errorMessages)
					{
						EAE6320_ASSERTF(false, reinterpret_cast<char*>(errorMessages->GetBufferPointer()));
						eae6320::Logging::OutputError("Direct3D failed to compile the vertex shader from the file %s: %s",
							sourceCodeFileName, reinterpret_cast<char*>(errorMessages->GetBufferPointer()));
						errorMessages->Release();
					}
					else
					{
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Direct3D failed to compile the vertex shader from the file %s",
							sourceCodeFileName);
					}
					return false;
				}
			}
			// Create the vertex shader object
			bool wereThereErrors = false;
			{
				ID3D11ClassLinkage* const noInterfaces = NULL;
				const HRESULT result = commonData->s_direct3dDevice->CreateVertexShader(o_compiledShader->GetBufferPointer(), o_compiledShader->GetBufferSize(),
					noInterfaces, &s_vertexShader);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the vertex shader with HRESULT %#010x", result);
					wereThereErrors = true;
				}
			}
			return !wereThereErrors;
		}*/
	{
		bool wereThereErrors = false;

		// Load the compiled shader
		const char* const path = "data/shaders/vertex.binshader";
		{
			std::string errorMessage;
			if (!eae6320::Platform::LoadBinaryFile(path, *o_compiledShader, &errorMessage))
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", path, errorMessage.c_str());
				goto OnExit;
			}
		}
		// Create the shader object
		{
			ID3D11ClassLinkage* const noInterfaces = NULL;
			const HRESULT result = commonData->s_direct3dDevice->CreateVertexShader(
				o_compiledShader->data, o_compiledShader->size, noInterfaces, &s_vertexShader);
			if (FAILED(result))
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", path, result);
				goto OnExit;
			}
		}

	OnExit:
		return !wereThereErrors;
	}
}
