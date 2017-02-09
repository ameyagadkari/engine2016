#ifndef EAE6320_IUICONTROLLER_H
#define EAE6320_IUICONTROLLER_H
#include <vector>

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class IUIController
			{
			public:
				virtual ~IUIController() = default;
				virtual void Draw() = 0;
				virtual void Update(std::string i_string) = 0;
				virtual void GetColor(float& i_r, float& i_g, float& i_b)const = 0;
				static void UpdateUIElements();
				static void ProcessInput();
			protected:
				IUIController() :isSelected(false) {}
				bool isSelected;
			private:			
				virtual void CleanUp() = 0;
				virtual void Initialize() = 0;
				static IUIController* currentSelectedUI;
				static size_t maxUIElements;
				static size_t currentSelectedUINumber;
			};
			

			extern const float colorManipulator;
			extern bool isDebugMenuEnabled;
			extern std::vector<IUIController*>debugUIs;		
		}
	}
}

#endif // !EAE6320_IUICONTROLLER_H