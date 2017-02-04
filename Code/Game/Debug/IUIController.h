#ifndef EAE6320_IUICONTROLLER_H
#define EAE6320_IUICONTROLLER_H

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
			private:						
				virtual void Initialize() = 0;			
			};
		}
	}
}

#endif // !EAE6320_IUICONTROLLER_H