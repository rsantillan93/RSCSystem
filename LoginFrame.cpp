// LoginFrame.cpp
#include "LoginFrame.h"

wxBEGIN_EVENT_TABLE(LoginFrame, wxFrame)
EVT_BUTTON(wxID_OK, LoginFrame::OnIngresar)
EVT_BUTTON(wxID_CANCEL, LoginFrame::OnCerrar)
EVT_TEXT_ENTER(wxID_ANY, LoginFrame::OnEnterPressed)
wxEND_EVENT_TABLE()

LoginFrame::LoginFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), hEnv(NULL), hDbc(NULL), conexionAbierta(false)
{
    // Set default font for the frame
    defaultFont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxStaticText* lblUsuario = new wxStaticText(panel, wxID_ANY, "Usuario:", wxPoint(30, 20));
    lblUsuario->SetFont(defaultFont);

    txtUsuario = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(30, 40), wxSize(220, 30), wxTE_PROCESS_ENTER);
    txtUsuario->SetFont(defaultFont);

    wxStaticText* lblContrasena = new wxStaticText(panel, wxID_ANY, "Contraseña:", wxPoint(30, 80));
    lblContrasena->SetFont(defaultFont);

    txtContrasena = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(30, 100), wxSize(220, 30), wxTE_PASSWORD | wxTE_PROCESS_ENTER);
    txtContrasena->SetFont(defaultFont);

    wxButton* btnIngresar = new wxButton(panel, wxID_OK, "Ingresar", wxPoint(30, 150), wxSize(100, 30));
    btnIngresar->SetFont(defaultFont);

    wxButton* btnCerrar = new wxButton(panel, wxID_CANCEL, "Cerrar", wxPoint(150, 150), wxSize(100, 30));
    btnCerrar->SetFont(defaultFont);

    Centre();

    // Iniciar conexión a la base de datos en un hilo separado
    std::thread conexionThread(&LoginFrame::IniciarConexionEnHilo, this);
    conexionThread.detach();
}

LoginFrame::~LoginFrame()
{
    CerrarConexion();
}

void LoginFrame::OnIngresar(wxCommandEvent& event)
{
    wxString usuario = txtUsuario->GetValue();
    wxString contrasena = txtContrasena->GetValue();

    if (usuario.IsEmpty() || contrasena.IsEmpty()) {
        wxMessageBox("Por favor, ingresa tu usuario y contraseña.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Autenticación del usuario en un hilo separado para no bloquear la interfaz
    std::thread autenticacionThread(&LoginFrame::AutenticarUsuarioEnHilo, this, usuario, contrasena);
    autenticacionThread.detach();
}

void LoginFrame::OnEnterPressed(wxCommandEvent& event)
{
    OnIngresar(event);
}

void LoginFrame::OnCerrar(wxCommandEvent& event)
{
    Close(true);
}

void LoginFrame::IniciarConexionEnHilo()
{
    // Intenta conectar a la base de datos y actualiza el estado de la conexión
    if (ConectarBaseDeDatos()) {
        conexionAbierta = true;
    }
    else {
        this->CallAfter([=]() {
            wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
            });
    }
}

bool LoginFrame::ConectarBaseDeDatos()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void LoginFrame::CerrarConexion()
{
    if (conexionAbierta) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        conexionAbierta = false;
    }
}

bool LoginFrame::AutenticarUsuario(SQLHDBC hDbc, const wxString& usuario, const wxString& contrasena)
{
    SQLHSTMT hStmt = NULL;
    SQLRETURN retcode;

    // Consulta SQL parametrizada
    wxString query = L"SELECT 1 FROM Usuarios WHERE Usuario = ? AND Contrasena = ?;";
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    retcode = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    // Vincular los parámetros de usuario y contraseña
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuario.length(), 0, (SQLPOINTER)usuario.wc_str(), 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, contrasena.length(), 0, (SQLPOINTER)contrasena.wc_str(), 0, NULL);

    // Ejecutar la consulta preparada
    retcode = SQLExecute(hStmt);

    // Verificar si los resultados son exitosos
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLFetch(hStmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return true; // Usuario autenticado
        }
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return false; // Fallo en la autenticación
}

void LoginFrame::AutenticarUsuarioEnHilo(const wxString& usuario, const wxString& contrasena)
{
    if (!conexionAbierta) {
        this->CallAfter([=]() {
            wxMessageBox("La conexión aún no está lista. Intenta de nuevo en unos segundos.", "Error", wxOK | wxICON_ERROR);
            });
        return;
    }

    bool resultado = AutenticarUsuario(hDbc, usuario, contrasena);

    this->CallAfter([=]() {
        if (resultado) {
            // Pasa el usuario autenticado a MainFrame
            MainFrame* mainFrame = new MainFrame("Sistema Hotelero", wxDefaultPosition, wxDefaultSize, usuario);
            mainFrame->Show(true);
            this->Close(true);
        }
        else {
            wxMessageBox("Usuario o contraseña incorrectos.", "Error", wxOK | wxICON_ERROR);
        }
        });
}
