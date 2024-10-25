#include "BuscarFrameHabitacion.h"
#include "HabitacionFrame.h"
#include <sqlext.h>

wxBEGIN_EVENT_TABLE(BuscarFrameHabitacion, wxFrame)
EVT_BUTTON(wxID_OK, BuscarFrameHabitacion::OnBuscar)
EVT_BUTTON(wxID_CANCEL, BuscarFrameHabitacion::OnCancelar)
EVT_CLOSE(BuscarFrameHabitacion::OnClose)
EVT_GRID_SELECT_CELL(BuscarFrameHabitacion::OnGridSelectCell)
EVT_GRID_CELL_LEFT_DCLICK(BuscarFrameHabitacion::OnGridCellDClick)  // Evento de doble clic
EVT_CHAR_HOOK(BuscarFrameHabitacion::OnKeyDown)  // Capturar la tecla ESC
wxEND_EVENT_TABLE()

BuscarFrameHabitacion::BuscarFrameHabitacion(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),
    mainFrameParent(parent)
{
    // Deshabilitar la ventana principal mientras esta ventana est� activa
    if (mainFrameParent != nullptr)
        mainFrameParent->Disable();

    // Configurar la fuente predeterminada
    wxFont defaultFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    // Crear un panel contenedor
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetFont(defaultFont);

    // Configurar los campos de entrada y etiquetas
    wxStaticText* lblNroOperacion = new wxStaticText(panel, wxID_ANY, "Nro Operacion:", wxPoint(10, 10));
    lblNroOperacion->SetFont(defaultFont);
    txtNroOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 7), wxSize(150, 25));

    btnBuscar = new wxButton(panel, wxID_OK, "Buscar", wxPoint(280, 7), wxSize(80, 30));
    btnCancelar = new wxButton(panel, wxID_CANCEL, "Cancelar", wxPoint(370, 7), wxSize(80, 30));

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 50));
    lblDescripcion->SetFont(defaultFont);
    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 47), wxSize(400, 25));

    // Configuraci�n del grid para mostrar resultados
    grid = new wxGrid(panel, wxID_ANY, wxPoint(10, 90), wxSize(700, 400));
    grid->CreateGrid(100, 3);  // 100 filas y 3 columnas
    grid->SetColLabelValue(0, "NroOperacion");
    grid->SetColLabelValue(1, "Descripcion");
    grid->SetColLabelValue(2, "Fecha creacion");

    // Configuraci�n de estilo del grid
    grid->SetDefaultCellFont(defaultFont);
    grid->SetLabelFont(defaultFont);
    grid->SetRowLabelSize(18);
    grid->SetSelectionMode(wxGrid::wxGridSelectRows);
    grid->EnableEditing(false);
    grid->EnableGridLines(true);
    grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    // Ajuste de tama�o de las columnas
    grid->SetColSize(0, 110);
    grid->SetColSize(1, 425);
    grid->SetColSize(2, 130);

    grid->SetGridLineColour(*wxLIGHT_GREY);
    grid->SetSelectionBackground(*wxWHITE);
    grid->SetSelectionForeground(*wxBLACK);

    // Manejador de eventos din�mico para cambios en las celdas
    grid->PushEventHandler(new wxEvtHandler);  // A�adir event handler din�mico a la grilla
    grid->Bind(wxEVT_GRID_CELL_CHANGED, [&](wxGridEvent& event) {
        int lastRow = grid->GetNumberRows() - 1;
        if (event.GetRow() == lastRow) {
            grid->AppendRows(1);  // A�adir fila si se edita la �ltima fila
        }
        event.Skip();  // Permitir que otros manejadores se ejecuten
        });

    // Inicializar etiquetas de filas sin selecci�n
    UpdateRowLabels(-1);

    // Ajustar el tama�o de la ventana seg�n el tama�o del padre
    this->SetSize(wxSize(parent->GetSize().GetWidth(), 550));
    CentreOnParent();  // Centrar esta ventana sobre la ventana principal

    // Establecer la conexi�n a la base de datos de forma segura
    if (!ConectarBaseDeDatos()) {
        wxMessageBox("No se pudo conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        Close();  // Cerrar la ventana si la conexi�n falla
    }
}

BuscarFrameHabitacion::~BuscarFrameHabitacion()
{
    CerrarConexion();  // Asegura la liberaci�n adecuada de recursos
}

void BuscarFrameHabitacion::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        // Evitar m�ltiples cierres si ya est� siendo eliminado
        if (!IsBeingDeleted()) {
            CerrarConexion();  // Asegurar la conexi�n se cierra

            if (mainFrameParent != nullptr) {
                auto habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
                if (habitacionFrame != nullptr) {
                    habitacionFrame->Enable();  // Rehabilitar la ventana principal
                }
            }

            Close();  // Usar Close para iniciar un cierre seguro (evita Destroy directo)
        }
    }
    else {
        event.Skip();  // Permitir que otros eventos de teclado sean manejados normalmente
    }
}

void BuscarFrameHabitacion::OnBuscar(wxCommandEvent& event)
{
    wxString nroOperacion = txtNroOperacion->GetValue();
    wxString descripcion = txtDescripcion->GetValue();

    // Limpiar la grilla antes de realizar una nueva b�squeda
    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    // Llamar a la funci�n para buscar operaciones en la base de datos
    BuscarOperacionesEnBaseDeDatos(nroOperacion, descripcion);
}

void BuscarFrameHabitacion::ActualizarNavegacionEnHabitacionFrame() {
    // Verificar si el mainFrameParent es v�lido
    auto habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
    if (habitacionFrame != nullptr) {
        // Obtener los IDs de las operaciones actuales
        int operacionActual = wxAtoi(habitacionFrame->GetOperacionID());
        int primeraOperacion = habitacionFrame->ObtenerMinimoOperacion();
        int ultimaOperacion = habitacionFrame->ObtenerMaximoOperacion();

        // Solo habilitar los botones si hay m�s de una operaci�n disponible
        bool esUnicaOperacion = (primeraOperacion == ultimaOperacion);

        habitacionFrame->GetButtonIzquierda()->Enable(!esUnicaOperacion && operacionActual != primeraOperacion);
        habitacionFrame->GetButtonDerecha()->Enable(!esUnicaOperacion && operacionActual != ultimaOperacion);

        habitacionFrame->GetExportarButton()->Enable(true);  // Habilitar el bot�n de exportaci�n
    }
}

void BuscarFrameHabitacion::OnCancelar(wxCommandEvent& event) {
    // Llamamos directamente a Close() para que se gestione el cierre de la ventana
    if (!IsBeingDeleted()) {
        Close(true);  // Usar Close en lugar de simular un evento de cierre
    }
}

void BuscarFrameHabitacion::OnClose(wxCloseEvent& event) {
    // Deshabilitar temporalmente el evento para evitar ciclos o m�ltiples llamadas
    this->Unbind(wxEVT_CLOSE_WINDOW, &BuscarFrameHabitacion::OnClose, this);

    if (!IsBeingDeleted()) {
        CerrarConexion();  // Asegurar que la conexi�n a la base de datos est� cerrada

        // Rehabilitar la ventana principal solo si a�n existe y no est� siendo eliminada
        if (mainFrameParent != nullptr && !mainFrameParent->IsBeingDeleted()) {
            auto habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
            if (habitacionFrame != nullptr) {
                habitacionFrame->Enable();  // Habilitar la ventana principal
                ActualizarNavegacionEnHabitacionFrame();  // Actualizar botones de navegaci�n
            }
        }

        // Destruir la ventana de forma segura
        if (!IsBeingDeleted()) {
            Destroy();  // Destruir la ventana de forma segura
        }
    }

    // Permitir que wxWidgets contin�e con el proceso de cierre
    event.Skip();
}

void BuscarFrameHabitacion::OnGridSelectCell(wxGridEvent& event)
{
    UpdateRowLabels(event.GetRow());
    event.Skip();
}

void BuscarFrameHabitacion::OnGridCellDClick(wxGridEvent& event) {
    int row = event.GetRow();
    wxString operacionID = grid->GetCellValue(row, 0);

    if (operacionID.IsEmpty()) {
        return;  // No hacer nada si la celda est� vac�a
    }

    if (mainFrameParent != nullptr && !mainFrameParent->IsBeingDeleted()) {
        auto habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
        if (habitacionFrame) {
            habitacionFrame->SetOperacionID(operacionID);
            habitacionFrame->GetGuardarButton()->Enable(false);
            ActualizarNavegacionEnHabitacionFrame();  // Actualizar botones despu�s de seleccionar operaci�n
        }
    }

    this->Unbind(wxEVT_GRID_CELL_LEFT_DCLICK, &BuscarFrameHabitacion::OnGridCellDClick, this);

    if (!IsBeingDeleted()) {
        this->Close(true);  // Cerrar la ventana de manera segura
    }
}

void BuscarFrameHabitacion::UpdateRowLabels(int selectedRow)
{
    for (int row = 0; row < grid->GetNumberRows(); ++row) { 
        grid->SetRowLabelValue(row, row == selectedRow ? ">" : "");
    }
}

bool BuscarFrameHabitacion::ConectarBaseDeDatos()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void BuscarFrameHabitacion::BuscarOperacionesEnBaseDeDatos(const wxString& nroOperacion, const wxString& descripcion)
{
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Construir la consulta SQL, agregando filtros solo si se proporcionan
    wxString query = L"SELECT DISTINCT OperacionID, Descripcion, FechaC FROM PrecioHabitacion WHERE 1=1";
    if (!nroOperacion.IsEmpty()) {
        query += wxString::Format(L" AND OperacionID LIKE '%%%s%%'", nroOperacion);
    }
    if (!descripcion.IsEmpty()) {
        query += wxString::Format(L" AND Descripcion LIKE '%%%s%%'", descripcion);
    }
    query += L" ORDER BY OperacionID;";

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        int row = 0;
        bool hasResults = false;

        // Procesar los resultados obtenidos
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            hasResults = true;

            SQLWCHAR operacionID[20], descripcion[255], fechaC[20];
            SQLGetData(hStmt, 1, SQL_C_WCHAR, operacionID, sizeof(operacionID), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, fechaC, sizeof(fechaC), NULL);

            if (row >= grid->GetNumberRows()) {
                grid->AppendRows(1);  // A�adir nuevas filas si es necesario
            }

            grid->SetCellValue(row, 0, wxString(operacionID));
            grid->SetCellValue(row, 1, wxString(descripcion));
            grid->SetCellValue(row, 2, wxString(fechaC));
            row++;
        }

        // Asegurarse de que el grid tenga al menos 100 filas, a�adiendo filas vac�as si es necesario
        int emptyRows = 100 - row;  // Calcular cu�ntas filas vac�as se necesitan
        if (emptyRows > 0) {
            grid->AppendRows(emptyRows);  // A�adir filas vac�as hasta completar 100 filas
        }

        // Refrescar la interfaz para asegurar que el grid no parezca bloqueado
        grid->ForceRefresh();
        wxYield();  // Permitir que la interfaz gr�fica se actualice antes de mostrar un mensaje

        // Si no se encontraron resultados, mostrar mensaje pero no borrar el grid
        if (!hasResults) {
            wxMessageBox("No se encontraron coincidencias para los criterios ingresados.", "Informaci�n", wxOK | wxICON_INFORMATION);
        }
    }
    else {
        // Refrescar el grid y la interfaz antes de mostrar el mensaje de error
        grid->ForceRefresh();
        wxYield();  // Permitir que la interfaz gr�fica se actualice antes de mostrar un mensaje

        wxMessageBox("Error al buscar en la base de datos.", "Error", wxOK | wxICON_ERROR);

        // Asegurarse de que el grid tenga al menos 100 filas visibles en caso de error
        if (grid->GetNumberRows() == 0) {
            grid->AppendRows(100);  // Mantener las filas visibles
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void BuscarFrameHabitacion::CerrarConexion()
{
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    }
    if (hEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}