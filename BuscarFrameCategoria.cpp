#include "BuscarFrameCategoria.h"
#include "CreacionCategoriaFrame.h"

wxBEGIN_EVENT_TABLE(BuscarFrameCategoria, wxFrame)
EVT_BUTTON(wxID_OK, BuscarFrameCategoria::OnBuscar)
EVT_BUTTON(wxID_CANCEL, BuscarFrameCategoria::OnCancelar)
EVT_CLOSE(BuscarFrameCategoria::OnClose)
EVT_GRID_CELL_LEFT_DCLICK(BuscarFrameCategoria::OnGridCellDClick)
EVT_GRID_SELECT_CELL(BuscarFrameCategoria::OnGridSelectCell)
EVT_CHAR_HOOK(BuscarFrameCategoria::OnKeyDown)  // Evento para capturar la tecla ESC
wxEND_EVENT_TABLE()

BuscarFrameCategoria::BuscarFrameCategoria(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),
    mainFrameParent(parent)
{
    if (mainFrameParent != nullptr)
        mainFrameParent->Disable();

    wxFont defaultFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetFont(defaultFont);

    // Campo para Operación
    wxStaticText* lblOperacion = new wxStaticText(panel, wxID_ANY, "Operacion:", wxPoint(10, 10));
    lblOperacion->SetFont(defaultFont);
    txtOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 7), wxSize(200, 25));
    txtOperacion->SetFont(defaultFont);

    // Campo para Descripción
    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 45));
    lblDescripcion->SetFont(defaultFont);
    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(120, 42), wxSize(400, 25));
    txtDescripcion->SetFont(defaultFont);

    // Botones de Buscar y Cancelar
    btnBuscar = new wxButton(panel, wxID_OK, "Buscar", wxPoint(530, 7), wxSize(80, 30));
    btnBuscar->SetFont(defaultFont);
    btnCancelar = new wxButton(panel, wxID_CANCEL, "Cancelar", wxPoint(620, 7), wxSize(80, 30));
    btnCancelar->SetFont(defaultFont);

    // Grid para mostrar resultados
    grid = new wxGrid(panel, wxID_ANY, wxPoint(10, 80), wxSize(700, 400));
    grid->CreateGrid(100, 2);
    grid->SetColLabelValue(0, "Operacion");
    grid->SetColLabelValue(1, "Descripcion");

    grid->SetDefaultCellFont(defaultFont);
    grid->SetLabelFont(defaultFont);
    grid->SetRowLabelSize(18);
    grid->SetSelectionMode(wxGrid::wxGridSelectRows);
    grid->EnableEditing(false);
    grid->EnableGridLines(true);
    grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    grid->SetColSize(0, 110);
    grid->SetColSize(1, 555);

    grid->SetGridLineColour(*wxLIGHT_GREY);
    grid->SetSelectionBackground(*wxWHITE);
    grid->SetSelectionForeground(*wxBLACK);

    UpdateRowLabels(-1);

    // Establecer el tamaño y centrar la ventana
    this->SetSize(wxSize(parent->GetSize().GetWidth(), 500));
    CentreOnParent();

    // Conectar la base de datos
    ConectarBaseDeDatos();
}

BuscarFrameCategoria::~BuscarFrameCategoria() {
    CerrarConexion();
}

void BuscarFrameCategoria::OnBuscar(wxCommandEvent& event)
{
    wxString operacion = txtOperacion->GetValue();
    wxString descripcion = txtDescripcion->GetValue();

    // Limpiar la grilla antes de hacer la búsqueda
    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    BuscarOperacionesEnBaseDeDatos(operacion, descripcion);
}

void BuscarFrameCategoria::OnCancelar(wxCommandEvent& event) {
    // Llamamos directamente a Close() para que se gestione el cierre de la ventana
    if (!IsBeingDeleted()) {
        Close(true);  // Usar Close en lugar de simular un evento de cierre
    }
}

void BuscarFrameCategoria::OnClose(wxCloseEvent& event) {
    // Deshabilitar temporalmente el manejador de eventos para evitar ciclos o múltiples llamadas
    this->Unbind(wxEVT_CLOSE_WINDOW, &BuscarFrameCategoria::OnClose, this);

    // Evitar múltiples intentos de cierre si la ventana ya está siendo eliminada
    if (!IsBeingDeleted()) {
        // Asegurar que la conexión a la base de datos esté cerrada
        CerrarConexion();

        // Rehabilitar la ventana principal si aún existe y no está siendo eliminada
        if (mainFrameParent != nullptr && !mainFrameParent->IsBeingDeleted()) {
            auto creacionFrame = dynamic_cast<CreacionCategoriaFrame*>(mainFrameParent);
            if (creacionFrame != nullptr) {
                creacionFrame->Enable();  // Habilitar la ventana principal
                creacionFrame->SetFocus();  // Traer la ventana principal al frente

                // Actualizar botones de navegación en la ventana principal
                creacionFrame->ActualizarEstadoBotonesNavegacionCategoria();
            }
        }

        // Destruir la ventana de manera segura solo si no ha sido eliminada previamente
        if (!IsBeingDeleted()) {
            Destroy();  // Destruir la ventana de forma segura
        }
    }

    // No volver a enlazar el manejador de eventos, ya que no es necesario una vez que la ventana comienza a cerrarse

    // Permitir que wxWidgets continúe con el proceso de cierre
    event.Skip();
}

void BuscarFrameCategoria::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        Close();  // Cerrar la ventana cuando se presiona ESC
    }
    else {
        event.Skip();  // Permitir que otros eventos de teclado sean manejados normalmente
    }
}

void BuscarFrameCategoria::OnGridCellDClick(wxGridEvent& event)
{
    int row = event.GetRow();

    // Obtener la Operación y Descripción seleccionadas del grid
    wxString operacionID = grid->GetCellValue(row, 0);
    wxString descripcion = grid->GetCellValue(row, 1);

    // Verificar si la celda de operación está vacía
    if (operacionID.IsEmpty()) {
        wxMessageBox("No se ha seleccionado una operación válida.", "Error", wxOK | wxICON_WARNING);
        return;
    }

    // Cast a la ventana principal (CreacionCategoriaFrame) y pasar la OperacionID y la Descripción
    CreacionCategoriaFrame* creacionFrame = dynamic_cast<CreacionCategoriaFrame*>(mainFrameParent);
    if (creacionFrame != nullptr)
    {
        // Pasar tanto la operación como la descripción para actualizar los campos en la ventana principal
        creacionFrame->BuscarOperacionCategoria(operacionID, descripcion);  // Llamar al método para cargar la operación y la descripción seleccionadas
        creacionFrame->Enable();  // Rehabilitar la ventana principal
        creacionFrame->SetFocus();  // Traer la ventana principal al frente

        // Actualizar los botones de navegación en la ventana principal
        creacionFrame->ActualizarEstadoBotonesNavegacionCategoria();  // Nuevo método para verificar si es la primera o la última operación
    }
    else
    {
        wxMessageBox("Error: No se pudo encontrar la ventana principal adecuada.", "Error", wxOK | wxICON_ERROR);
    }

    Close();  // Cerrar la ventana de búsqueda después de la selección
}

void BuscarFrameCategoria::OnGridSelectCell(wxGridEvent& event)
{
    int row = event.GetRow();
    UpdateRowLabels(row);
    event.Skip();
}

void BuscarFrameCategoria::UpdateRowLabels(int selectedRow)
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

bool BuscarFrameCategoria::ConectarBaseDeDatos()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void BuscarFrameCategoria::BuscarOperacionesEnBaseDeDatos(const wxString& operacion, const wxString& descripcion)
{
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Construir la consulta SQL
    wxString query = L"SELECT Operacion, Descripcion FROM Categoria WHERE 1=1";

    // Verificar si el campo operacion contiene un número válido o una cadena no vacía
    long operacionNumber;
    bool operacionEsNumero = operacion.ToLong(&operacionNumber);

    if (!operacion.IsEmpty()) {
        if (operacionEsNumero) {
            query += wxString::Format(L" AND Operacion = %ld", operacionNumber);
        }
        else {
            query += wxString::Format(L" AND Operacion LIKE '%%%s%%'", operacion);
        }
    }

    // Agregar la condición de descripción si no está vacía
    if (!descripcion.IsEmpty()) {
        query += wxString::Format(L" AND Descripcion LIKE '%%%s%%'", descripcion);
    }

    query += L" ORDER BY Operacion;";

    // Limpiar el grid antes de ejecutar la consulta
    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int row = 0;
    bool hasResults = false;  // Variable para verificar si hay resultados

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        // Procesar los resultados obtenidos
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            hasResults = true;

            SQLWCHAR operacion[20], descripcion[255];
            SQLGetData(hStmt, 1, SQL_C_WCHAR, operacion, sizeof(operacion), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);

            if (row >= grid->GetNumberRows()) {
                grid->AppendRows(1);  // Agregar una fila nueva si es necesario
            }

            grid->SetCellValue(row, 0, wxString(operacion));
            grid->SetCellValue(row, 1, wxString(descripcion));

            row++;
        }

        // Asegurarse de que el grid tenga 100 filas visibles
        int emptyRows = 100 - row;
        if (emptyRows > 0) {
            grid->AppendRows(emptyRows);
        }

    }
    else {
        wxMessageBox("Error al buscar en la base de datos.", "Error", wxOK | wxICON_ERROR);

        // Asegurarse de que el grid tenga 100 filas visibles en caso de error
        if (grid->GetNumberRows() == 0) {
            grid->AppendRows(100);
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Refrescar el grid para asegurarse de que se vea correctamente
    grid->ForceRefresh();
    wxYield();

    // Mostrar un mensaje si no se encontraron resultados
    if (!hasResults) {
        wxMessageBox("No se encontraron coincidencias para los criterios ingresados.",
            "Información", wxOK | wxICON_INFORMATION);
    }
}

void BuscarFrameCategoria::CerrarConexion() {
    if (hDbc != SQL_NULL_HDBC) {  // Verificar y cerrar la conexión a la base de datos
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = SQL_NULL_HDBC;  // Marcar el manejador como liberado
    }
    if (hEnv != SQL_NULL_HENV) {  // Liberar el entorno de la conexión
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = SQL_NULL_HENV;  // Marcar el manejador como liberado
    }
}