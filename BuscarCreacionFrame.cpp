#include "BuscarCreacionFrame.h"
#include "CreacionHabitacionesFrame.h"

wxBEGIN_EVENT_TABLE(BuscarCreacionFrame, wxFrame)
EVT_BUTTON(wxID_OK, BuscarCreacionFrame::OnBuscar)
EVT_BUTTON(wxID_CANCEL, BuscarCreacionFrame::OnCancelar)
EVT_CLOSE(BuscarCreacionFrame::OnClose)
EVT_GRID_CELL_LEFT_DCLICK(BuscarCreacionFrame::OnGridCellDClick)
EVT_GRID_SELECT_CELL(BuscarCreacionFrame::OnGridSelectCell)  // Nuevo evento agregado
EVT_CHAR_HOOK(BuscarCreacionFrame::OnKeyDown)  // Nuevo evento para la tecla ESC
wxEND_EVENT_TABLE()

BuscarCreacionFrame::BuscarCreacionFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),  // Deshabilitar maximizar
    mainFrameParent(parent)
{
    if (mainFrameParent != nullptr)
        mainFrameParent->Disable();

    wxFont defaultFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetFont(defaultFont);

    wxStaticText* lblIDHab = new wxStaticText(panel, wxID_ANY, "ID Hab:", wxPoint(10, 10));
    lblIDHab->SetFont(defaultFont);
    txtNroOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 7), wxSize(150, 25));
    txtNroOperacion->SetFont(defaultFont);

    btnBuscar = new wxButton(panel, wxID_OK, "Buscar", wxPoint(280, 7), wxSize(80, 30));
    btnBuscar->SetFont(defaultFont);
    btnCancelar = new wxButton(panel, wxID_CANCEL, "Cancelar", wxPoint(370, 7), wxSize(80, 30));
    btnCancelar->SetFont(defaultFont);

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 50));
    lblDescripcion->SetFont(defaultFont);
    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 47), wxSize(400, 25));
    txtDescripcion->SetFont(defaultFont);

    grid = new wxGrid(panel, wxID_ANY, wxPoint(10, 90), wxSize(700, 400));
    grid->CreateGrid(100, 3);  // Cambiar de 4 a 3 columnas visibles
    grid->SetColLabelValue(0, "IDHab");
    grid->SetColLabelValue(1, "Descripcion");
    grid->SetColLabelValue(2, "UM");

    grid->SetDefaultCellFont(defaultFont);
    grid->SetLabelFont(defaultFont);
    grid->SetRowLabelSize(18);
    grid->SetSelectionMode(wxGrid::wxGridSelectRows);
    grid->EnableEditing(false);
    grid->EnableGridLines(true);
    grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    grid->SetColSize(0, 60);
    grid->SetColSize(1, 555);  // Ajustar el tamaño de la columna Descripcion
    grid->SetColSize(2, 50);   // Ajustar el tamaño de la columna UM

    grid->SetGridLineColour(*wxLIGHT_GREY);
    grid->SetSelectionBackground(*wxWHITE);
    grid->SetSelectionForeground(*wxBLACK);

    UpdateRowLabels(-1);

    this->SetSize(wxSize(parent->GetSize().GetWidth(), 550));
    CentreOnParent();

    ConectarBaseDeDatos();
}

BuscarCreacionFrame::~BuscarCreacionFrame() {
    CerrarConexion();
}

void BuscarCreacionFrame::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        Close();  // Cerrar la ventana cuando se presiona ESC
    }
    else {
        event.Skip();  // Dejar que otros eventos de teclas se procesen normalmente
    }
}

void BuscarCreacionFrame::OnBuscar(wxCommandEvent& event)
{
    wxString nroOperacion = txtNroOperacion->GetValue();
    wxString descripcion = txtDescripcion->GetValue();

    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    BuscarOperacionesEnBaseDeDatos(nroOperacion, descripcion);
}

void BuscarCreacionFrame::OnCancelar(wxCommandEvent& event) {
    // Llamamos directamente a Close() para que se gestione el cierre de la ventana
    if (!IsBeingDeleted()) {
        Close(true);  // Usar Close en lugar de simular un evento de cierre
    }
}

void BuscarCreacionFrame::OnClose(wxCloseEvent& event)
{
    // Deshabilitar temporalmente el evento de cierre para evitar múltiples llamadas
    this->Unbind(wxEVT_CLOSE_WINDOW, &BuscarCreacionFrame::OnClose, this);

    // Evitar múltiples intentos de cierre si la ventana ya está siendo eliminada
    if (!IsBeingDeleted()) {
        CerrarConexion();  // Asegurar que la conexión a la base de datos se cierra

        // Rehabilitar la ventana principal solo si aún existe y no está siendo eliminada
        if (mainFrameParent != nullptr && !mainFrameParent->IsBeingDeleted())
        {
            auto creacionFrame = dynamic_cast<CreacionHabitacionesFrame*>(mainFrameParent);
            if (creacionFrame != nullptr)
            {
                creacionFrame->Enable();  // Rehabilitar la ventana principal
                creacionFrame->SetFocus();  // Traer la ventana principal al frente

                // Actualizar los botones de navegación en la ventana principal
                ActualizarNavegacionEnCreacionFrame();
            }
        }

        // Destruir la ventana de forma segura solo si no ha sido eliminada previamente
        if (!IsBeingDeleted()) {
            Destroy();  // Iniciar la destrucción segura de la ventana
        }
    }

    // No volver a enlazar el evento de cierre, ya que no es necesario una vez que la ventana comienza a cerrarse

    // Permitir que wxWidgets continúe con cualquier manejo adicional del evento
    event.Skip();
}

void BuscarCreacionFrame::ActualizarNavegacionEnCreacionFrame()
{
    CreacionHabitacionesFrame* creacionFrame = dynamic_cast<CreacionHabitacionesFrame*>(mainFrameParent);
    if (creacionFrame != nullptr)
    {
        int currentID = wxAtoi(creacionFrame->GetTxtNroOperacion()->GetValue());
        int firstID = creacionFrame->ObtenerPrimerIDHab();
        int lastID = creacionFrame->ObtenerUltimoIDHab();

        // Deshabilitar o habilitar botones según si es la primera o última operación
        if (currentID == firstID) {
            creacionFrame->GetBtnIzquierda()->Enable(false);
        }
        else {
            creacionFrame->GetBtnIzquierda()->Enable(true);
        }

        if (currentID == lastID) {
            creacionFrame->GetBtnDerecha()->Enable(false);
        }
        else {
            creacionFrame->GetBtnDerecha()->Enable(true);
        }
    }
}

void BuscarCreacionFrame::OnGridCellDClick(wxGridEvent& event)
{
    int row = event.GetRow();
    wxString idHab = grid->GetCellValue(row, 0); // Obtener el IDHab seleccionado de la grilla

    // Verificar si la celda está vacía
    if (idHab.IsEmpty()) {
        grid->ForceRefresh();
        wxYield();
        wxMessageBox("La celda seleccionada está vacía.", "Información", wxOK | wxICON_INFORMATION);
        return;
    }

    wxString operacionID = obtenerOperacionIDPorIDHab(idHab);  // Buscar el OperacionID correspondiente

    CreacionHabitacionesFrame* creacionFrame = dynamic_cast<CreacionHabitacionesFrame*>(mainFrameParent);
    if (creacionFrame != nullptr)
    {
        creacionFrame->SetIDHab(operacionID); // Establecer el OperacionID

        // Actualizar los botones de navegación
        ActualizarNavegacionEnCreacionFrame();

        // Habilitar la ventana principal
        creacionFrame->Enable();
        creacionFrame->SetFocus(); // Traer la ventana principal al frente
    }
    else
    {
        grid->ForceRefresh();
        wxYield();
        wxMessageBox("Error: No se pudo encontrar la ventana principal adecuada.", "Error", wxOK | wxICON_ERROR);
    }

    Close(); // Cierra la ventana de búsqueda
}

void BuscarCreacionFrame::OnGridSelectCell(wxGridEvent& event)
{
    int row = event.GetRow();
    UpdateRowLabels(row);
    event.Skip();
}

void BuscarCreacionFrame::UpdateRowLabels(int selectedRow)
{
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        if (row == selectedRow) {
            grid->SetRowLabelValue(row, ">");
        }
        else {
            grid->SetRowLabelValue(row, "");
        }
    }
}

bool BuscarCreacionFrame::ConectarBaseDeDatos()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void BuscarCreacionFrame::BuscarOperacionesEnBaseDeDatos(const wxString& nroOperacion, const wxString& descripcion)
{
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = L"SELECT DISTINCT OperacionID, IdHab, Descripcion, UM FROM Habitacion WHERE 1=1";
    if (!nroOperacion.IsEmpty()) {
        query += wxString::Format(L" AND IdHab LIKE '%%%s%%'", nroOperacion);
    }
    if (!descripcion.IsEmpty()) {
        query += wxString::Format(L" AND Descripcion LIKE '%%%s%%'", descripcion);
    }
    query += L" ORDER BY IdHab;";

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int row = 0;
    bool hasResults = false; // Variable para verificar si hay resultados

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            hasResults = true; // Si se obtiene al menos un resultado

            SQLWCHAR operacionID[20], idHab[255], descripcion[255], um[50];
            SQLGetData(hStmt, 1, SQL_C_WCHAR, operacionID, sizeof(operacionID), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, idHab, sizeof(idHab), NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, um, sizeof(um), NULL);

            if (row >= grid->GetNumberRows()) {
                grid->AppendRows(1);
            }

            grid->SetCellValue(row, 0, wxString(idHab));
            grid->SetCellValue(row, 1, wxString(descripcion));
            grid->SetCellValue(row, 2, wxString(um));

            // Guardar OperacionID en un mapa interno para usarlo al seleccionar
            operacionIdMap[wxString(idHab)] = wxString(operacionID);

            row++;
        }

        // Si hay menos de 100 filas, agregar filas vacías hasta tener 100 filas visibles
        int emptyRows = 100 - row;
        if (emptyRows > 0) {
            grid->AppendRows(emptyRows);
        }

    }
    else {
        // Refrescar la interfaz y el grid antes de mostrar el mensaje de error
        grid->ForceRefresh();
        wxYield();

        wxMessageBox("Error al buscar en la base de datos.", "Error", wxOK | wxICON_ERROR);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Refrescar el grid para asegurarse de que se vea correctamente
    grid->ForceRefresh();
    wxYield();

    // Si no se encontraron resultados, mostrar un mensaje de información
    if (!hasResults) {
        wxMessageBox("No se encontraron coincidencias para los criterios ingresados.", "Información", wxOK | wxICON_INFORMATION);
    }

    // Actualizar las etiquetas de fila después de la búsqueda
    UpdateRowLabels(-1);
}

void BuscarCreacionFrame::CerrarConexion()
{
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = SQL_NULL_HDBC;  // Evitar reuso del manejador
    }
    if (hEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = SQL_NULL_HENV;  // Evitar reuso del manejador
    }
}

wxString BuscarCreacionFrame::obtenerOperacionIDPorIDHab(const wxString& idHab)
{
    if (operacionIdMap.find(idHab) != operacionIdMap.end()) {
        return operacionIdMap[idHab];
    }
    return wxEmptyString;
}
