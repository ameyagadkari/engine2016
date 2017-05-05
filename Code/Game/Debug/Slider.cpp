#include "Slider.h"
#include "Text.h"
#include "../../Game/Gameplay/GameObject2D.h"
#include "../../Engine/Graphics/cSprite.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Graphics/Font.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"

namespace
{
	const std::string sliderBarPath = "data/gameobjects2d/ui/sliderbar.bing2dobj";
}
eae6320::Debug::UI::Slider::Slider(
	const PixelCoordinates i_pixelCoordinates, 
	const std::string i_sliderName, 
	const Color i_color, 
	const float i_value,
	const float i_minValue, 
	const float i_maxValue, 
	const float i_sliderSize, 
	void const * i_callbackThisPointer, 
	float(*i_callback)(void const* i_callbackthisPointer, const float i_currentValue, const float i_minvalue, const float i_maxvalue), 
	const bool i_isSelected)
	:
	IUIController(i_color, i_isSelected),
	m_callback(i_callback),
	m_callbackThisPointer(i_callbackThisPointer),
	m_sliderName(new Text(i_pixelCoordinates, i_sliderName, i_color)),
	m_value(i_value),
	m_minValue(i_minValue),
	m_maxValue(i_maxValue),
	m_sliderSize(i_sliderSize),
	m_pixelCoordinates(i_pixelCoordinates)
{
	EAE6320_ASSERTF((i_maxValue >= i_value) && (i_value >= i_minValue), "Incorrect min-max values or current value for a slider");
	Slider::Initialize();
}

eae6320::Debug::UI::Slider::~Slider()
{
	Slider::CleanUp();
}
float eae6320::Debug::UI::Slider::GetValue() const
{
	return m_value;
}

void eae6320::Debug::UI::Slider::Initialize()
{
	const bool doNotUsePixelCoordinatesFromFile = true;
	if (Graphics::Font::widthArray && Graphics::Font::texcoordArray)
	{
		m_slider = Gameplay::GameObject2D::LoadGameObject2D(sliderBarPath.c_str(), doNotUsePixelCoordinatesFromFile, m_sliderName->GetRightTextPixelCoordinate(), m_pixelCoordinates.y);
		RecalculateSliderbarWidth();
	}
	else
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Font not initialized so sliders cannot be initialized");
	}
}

void eae6320::Debug::UI::Slider::CleanUp()
{

	if (m_slider)
	{
		delete m_slider;
		m_slider = nullptr;
	}

	if (m_sliderName)
	{
		delete m_sliderName;
		m_sliderName = nullptr;
	}
}

void eae6320::Debug::UI::Slider::Draw(const Graphics::Material * const i_material, const float alpha, const bool invertColor) const
{
	IUIController* text = reinterpret_cast<IUIController*>(m_sliderName);
	text->isSelected = isSelected;
	text->Draw();
	IUIController::Draw(m_slider->GetMaterial());
	m_slider->GetSprite()->Draw();
}

void eae6320::Debug::UI::Slider::Update()
{
	if(isSelected && m_callback)m_value = m_callback(m_callbackThisPointer, m_value, m_minValue, m_maxValue);
	RecalculateSliderbarWidth();

	/*if (isSelected)
	{
		float offsetModifier;
		if (UserInput::GetKey(VK_LEFT))
		{
			offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
			m_value = (m_value > m_minValue) ? m_value - offsetModifier : m_minValue;
			RecalculateSliderbarWidth();
		}
		if (UserInput::GetKey(VK_RIGHT))
		{
			offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
			m_value = (m_value < m_maxValue) ? m_value + offsetModifier : m_maxValue;
			RecalculateSliderbarWidth();
		}
	}*/
}

void eae6320::Debug::UI::Slider::RecalculateSliderbarWidth() const
{
	Gameplay::RectTransform rectTransform = m_slider->GetRectTransform();
	const Gameplay::Anchor anchor = m_slider->GetAnchor();
	rectTransform.width = static_cast<uint16_t>((m_value - m_minValue) / (m_maxValue - m_minValue) * m_sliderSize);
	const Graphics::Sprite::ScreenPosition screenPosition = ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(rectTransform, anchor);
	m_slider->GetSprite()->SetScreenPosition(screenPosition);
}

void eae6320::Debug::UI::Slider::Reset(float i_resetValue)
{
	m_value = i_resetValue;
	RecalculateSliderbarWidth();
}
