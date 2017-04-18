#include "Checkbox.h"
#include "Text.h"
#include "../../Game/Gameplay/GameObject2D.h"
#include "../../Engine/Graphics/cSprite.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Graphics/Font.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"

namespace
{
	const int16_t xTextOffset = 48;
	const std::string checkedFilePath = "data/gameobjects2d/ui/checked.bing2dobj";
	const std::string uncheckedFilePath = "data/gameobjects2d/ui/unchecked.bing2dobj";
}

eae6320::Debug::UI::Checkbox::~Checkbox()
{
	Checkbox::CleanUp();
}

eae6320::Debug::UI::Checkbox::Checkbox(const PixelCoordinates i_pixelCoordinates, const std::string i_onText, const std::string i_offText, const Color i_color, const bool i_isOn, void(*i_callback)(const bool), const bool i_isSelected) :
	IUIController(i_color, i_isSelected),
	m_pixelCoordinates(i_pixelCoordinates),
	m_callback(i_callback),
	m_onText(new Text({ i_pixelCoordinates.x + xTextOffset,i_pixelCoordinates.y }, i_onText, i_color)),
	m_offText(new Text({ i_pixelCoordinates.x + xTextOffset,i_pixelCoordinates.y }, i_offText, i_color)),m_isOn(i_isOn)
{
	Checkbox::Initialize();
}

void eae6320::Debug::UI::Checkbox::Initialize()
{
	const bool doNotUsePixelCoordinatesFromFile = true;
	if (Graphics::Font::widthArray && Graphics::Font::texcoordArray)
	{
		m_checked = Gameplay::GameObject2D::LoadGameObject2D(checkedFilePath.c_str(), doNotUsePixelCoordinatesFromFile, m_pixelCoordinates.x, m_pixelCoordinates.y);
		m_unchecked = Gameplay::GameObject2D::LoadGameObject2D(uncheckedFilePath.c_str(), doNotUsePixelCoordinatesFromFile, m_pixelCoordinates.x, m_pixelCoordinates.y);
	}
	else
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Font not initialized so check boxes cannot be initialized");
	}
}
void eae6320::Debug::UI::Checkbox::CleanUp()
{

	if (m_checked)
	{
		delete m_checked;
		m_checked = nullptr;
	}
	if (m_unchecked)
	{
		delete m_unchecked;
		m_unchecked = nullptr;
	}

	if (m_onText)
	{
		delete m_onText;
		m_onText = nullptr;
	}

	if (m_offText)
	{
		delete m_offText;
		m_offText = nullptr;
	}
}

void eae6320::Debug::UI::Checkbox::Update()
{
	if (isSelected && UserInput::GetKeyDown(VK_RETURN))
	{
		m_isOn = !m_isOn;
	}
	if (m_callback)m_callback(m_isOn);
}
void eae6320::Debug::UI::Checkbox::Draw(const Graphics::Material* const i_material, const float alpha, const bool invertColor)const
{
	if (m_isOn)
	{
		IUIController* text = reinterpret_cast<IUIController*>(m_onText);
		text->isSelected = isSelected;
		text->Draw();
		IUIController::Draw(m_checked->GetMaterial());
		m_checked->GetSprite()->Draw();
	}
	else
	{
		IUIController* text = reinterpret_cast<IUIController*>(m_offText);
		text->isSelected = isSelected;
		text->Draw();
		IUIController::Draw(m_unchecked->GetMaterial());
		m_unchecked->GetSprite()->Draw();
	}
}
