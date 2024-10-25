#include "MyApp.h"
#include "LoginFrame.h"

// Ensure wxWidgets macros are correctly used
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    // Create and show the login frame
    LoginFrame* loginFrame = new LoginFrame("Ingresar al Sistema", wxPoint(50, 50), wxSize(300, 250));
    loginFrame->Show(true);
    return true;
}
