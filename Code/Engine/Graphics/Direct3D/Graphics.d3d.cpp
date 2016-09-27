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
// Static Data Initialization
//===========================

namespace
{
	using namespace eae6320::Graphics;
	CommonData *commonData = CommonData::GetCommonData();
	std::vector<eae6320::Gameplay::GameObject*> gameObjects;
	// This is the main window handle from Windows
	HWND s_renderingWindow = NULL;
	// These are D3D interfaces
	//ID3D11Device* s_direct3dDevice = NULL;
	IDXGISwapChain* s_swapChain = NULL;
	//ID3D11DeviceContext* s_direct3dImmediateContext = NULL;
	ID3D11RenderTargetView* s_renderTargetView = NULL;
	// The vertex buffer holds the data for each vertex
	ID3D11Buffer* s_vertexBuffer = NULL;

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
	//struct
	//{
	//	union
	//	{
	//		float g_elapsedSecondCount_total;
	//		float register0[4];	// You won't have to worry about why I do this until a later assignment
	//	};
	//} s_constantBufferData;
	//ID3D11Buffer* s_constantBuffer = NULL;

	ConstantBufferData::sFrame frameBufferData;
	ConstantBufferData::sDrawCall drawCallBufferData;
	ConstantBuffer frameBuffer;
	ConstantBuffer drawCallBuffer;
}
// Helper Function Declarations
//=============================

namespace
{
	//bool CreateConstantBuffer();
	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool LoadFragmentShader();
	bool LoadVertexShader(ID3D10Blob*& o_compiledShader);
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

	// Update the constant buffer
	frameBuffer.UpdateConstantBuffer(&frameBufferData, sizeof(frameBufferData));
	//{
	//	// Update the struct (i.e. the memory that we own)
	//	frameBufferData.g_elapsedSecondCount_total = Time::GetElapsedSecondCount_total();
	//	// Get a pointer from Direct3D that can be written to
	//	void* memoryToWriteTo = NULL;
	//	{
	//		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//		{
	//			// Discard previous contents when writing
	//			const unsigned int noSubResources = 0;
	//			const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	//			const unsigned int noFlags = 0;
	//			const HRESULT result = commonData->s_direct3dImmediateContext->Map(s_constantBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
	//			if (SUCCEEDED(result))
	//			{
	//				memoryToWriteTo = mappedSubResource.pData;
	//			}
	//			else
	//			{
	//				EAE6320_ASSERT(false);
	//			}
	//		}
	//	}
	//	if (memoryToWriteTo)
	//	{
	//		// Copy the new data to the memory that Direct3D has provided
	//		memcpy(memoryToWriteTo, &s_constantBufferData, sizeof(s_constantBufferData));
	//		// Let Direct3D know that the memory contains the data
	//		// (the pointer will be invalid after this call)
	//		const unsigned int noSubResources = 0;
	//		commonData->s_direct3dImmediateContext->Unmap(s_constantBuffer, noSubResources);
	//		memoryToWriteTo = NULL;
	//	}
	//	 Bind the constant buffer to the shader
	//	{
	//		/*const unsigned int registerAssignedInShader = 0;
	//		const unsigned int bufferCount = 1;
	//		commonData->s_direct3dImmediateContext->VSSetConstantBuffers( registerAssignedInShader, bufferCount, &s_constantBuffer );
	//		commonData->s_direct3dImmediateContext->PSSetConstantBuffers( registerAssignedInShader, bufferCount, &s_constantBuffer );*/
	//	}
	//}

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
		for (size_t i = 0; i < numberOfMeshes; i++)
		{
			drawCallBuffer.UpdateConstantBuffer(&gameObjects[i]->drawCallBufferData, sizeof(gameObjects[i]->drawCallBufferData));
			gameObjects[i]->mesh->RenderMesh();
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
	if (!CreateView(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight))
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
		commonData->compiledVertexShader->Release();
		commonData->compiledVertexShader = NULL;
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
			wereThereErrors = true;
		}
		if (!drawCallBuffer.CleanUpConstantBuffer())
		{
			wereThereErrors = true;
		}
		if (s_renderTargetView)
		{
			s_renderTargetView->Release();
			s_renderTargetView = NULL;
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

void eae6320::Graphics::SetGameObject(Gameplay::GameObject*GameObject, const float x, const float y)
{	
	gameObjects.push_back(GameObject);
	GameObject->SetNewInitialOffset(x, y);
}

// Helper Function Definitions
//============================

namespace
{
	//bool CreateConstantBuffer()
	//{
	//	D3D11_BUFFER_DESC bufferDescription = { 0 };
	//	{
	//		// The byte width must be rounded up to a multiple of 16
	//		bufferDescription.ByteWidth = sizeof(s_constantBufferData);
	//		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	// The CPU must be able to update the buffer
	//		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU must write, but doesn't read
	//		bufferDescription.MiscFlags = 0;
	//		bufferDescription.StructureByteStride = 0;	// Not used
	//	}
	//	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	//	{
	//		// Fill in the constant buffer
	//		s_constantBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	//		initialData.pSysMem = &s_constantBufferData;
	//		// (The other data members are ignored for non-texture buffers)
	//	}

	//	const HRESULT result = commonData->s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_constantBuffer);
	//	if (SUCCEEDED(result))
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		EAE6320_ASSERT(false);
	//		eae6320::Logging::OutputError("Direct3D failed to create a constant buffer with HRESULT %#010x", result);
	//		return false;
	//	}
	//}

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

	bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
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
	}

	bool LoadFragmentShader()
	{
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
	}

	bool LoadVertexShader(ID3D10Blob*& o_compiledShader)
	{
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
	}
}
