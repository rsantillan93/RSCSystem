#include "GridBuscarIDHabFrame.h"
#include "HabitacionFrame.h"

wxBEGIN_EVENT_TABLE(GridBuscarIDHabFrame, wxFrame)
EVT_BUTTON(wxID_OK, GridBuscarIDHabFrame::OnBuscar)
EVT_BUTTON(wxID_CANCEL, GridBuscarIDHabFrame::OnCancelar)
EVT_CLOSE(GridBuscarIDHabFrame::OnClose)
EVT_GRID_CELL_LEFT_DCLICK(GridBuscarIDHabFrame::OnGridCellDClick)
EVT_GRID_SELECT_CELL(GridBuscarIDHabFrame::OnGridSelectCell)  // Añadir el evento de selección de celda
EVT_CHAR_HOOK(GridBuscarIDHabFrame::OnKeyDown)  // Capturar la tecla ESC
wxEND_EVENT_TABLE()

GridBuscarIDHabFrame::GridBuscarIDHabFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),  // Agregar los estilos aquí
    mainFrameParent(parent)
{
    if (mainFrameParent != nullptr) {
        HabitacionFrame* habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
        if (habitacionFrame != nullptr) {
            habitacionFrame->GetGrid()->Disable();  // Desactivar el grid del padre para evitar conflictos
        }
    }

    if (mainFrameParent != nullptr)
        mainFrameParent->Disable();

    wxFont defaultFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
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

    CentreOnParent();
    grid = new wxGrid(panel, wxID_ANY, wxPoint(10, 90), wxSize(700, 400));
    grid->CreateGrid(100, 2);  // Cambiar de 3 a 2 columnas
    grid->SetColLabelValue(0, "IDHab");
    grid->SetColLabelValue(1, "Descripcion");

    grid->SetDefaultCellFont(defaultFont);
    grid->SetLabelFont(defaultFont);
    grid->SetRowLabelSize(18);
    grid->SetSelectionMode(wxGrid::wxGridSelectCells);
    grid->EnableEditing(false);
    grid->EnableGridLines(true);
    grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    grid->SetColSize(0, 60);   // Ajustar tamaño de columna IDHab
    grid->SetColSize(1, 605);  // Ajustar tamaño de columna Descripcion

    ConectarBaseDeDatos();

    // Llamar a UpdateRowLabels para asegurarse de que las filas se inicializan sin símbolos de selección
    UpdateRowLabels(-1);
}

GridBuscarIDHabFrame::~GridBuscarIDHabFrame() {
    CerrarConexion();  // Asegurarse de que los handles estén liberados correctamente
}

void GridBuscarIDHabFrame::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        Close();  // Cerrar la ventana de manera segura cuando se presiona ESC
    }
    else {
        event.Skip();  // Permitir que otros eventos de teclado sean manejados normalmente
    }
}

void GridBuscarIDHabFrame::OnBuscar(wxCommandEvent& event) {
    // Desactivar los eventos del grid de HabitacionFrame mientras se interactúa con GridBuscarIDHabFrame
    HabitacionFrame* habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
    if (habitacionFrame != nullptr) {
        habitacionFrame->DesactivarEventosGrid();
    }
    wxString idHab = txtNroOperacion->GetValue();
    wxString descripcion = txtDescripcion->GetValue();

    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    BuscarHabitacionesEnBaseDeDatos(idHab, descripcion);
}

void GridBuscarIDHabFrame::OnCancelar(wxCommandEvent& event)
{
    // Simplemente llamar a Close para desencadenar el evento OnClose
    Close();
}

void GridBuscarIDHabFrame::OnClose(wxCloseEvent& event)
{
    if (!IsBeingDeleted()) {
        CerrarConexion();  // Asegurar que la conexión se cierra correctamente

        if (mainFrameParent != nullptr) {
            HabitacionFrame* habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
            if (habitacionFrame != nullptr) {
                if (habitacionFrame->GetGrid() && habitacionFrame->GetGrid()->IsShown()) {
                    habitacionFrame->GetGrid()->Enable();   // Reactivar el grid del padre
                    habitacionFrame->ActivarEventosGrid();  // Reactivar los eventos del grid del padre
                }
            }
            mainFrameParent->Enable();  // Habilitar la ventana principal
        }

        Destroy();  // Destruir esta ventana para liberar los recursos
    }

    event.Skip();  // Permitir que wxWidgets maneje cualquier procesamiento adicional
}

void GridBuscarIDHabFrame::OnGridCellDClick(wxGridEvent& event)
{
    int row = event.GetRow();  // Obtener la fila seleccionada

    // Verificar si la celda tiene datos en "IDHab" (columna 0)
    wxString idHabValue = grid->GetCellValue(row, 0);
    if (idHabValue.IsEmpty()) {
        // Refrescar la interfaz antes de mostrar el mensaje
        grid->ForceRefresh();
        wxYield();

        wxMessageBox("La celda seleccionada está vacía.", "Información", wxOK | wxICON_INFORMATION);
        return;
    }

    // Obtener los valores necesarios de la fila seleccionada
    wxString descripcionValue = grid->GetCellValue(row, 1); // Valor de Descripcion (columna 1)

    // Verificar si la ventana principal es un HabitacionFrame
    HabitacionFrame* habitacionFrame = dynamic_cast<HabitacionFrame*>(mainFrameParent);
    if (habitacionFrame != nullptr) {
        // Verificar si el IDHab seleccionado ya existe en el grid de HabitacionFrame
        if (habitacionFrame->IsIDHabDuplicated(idHabValue)) {
            // Refrescar la interfaz antes de mostrar el mensaje
            grid->ForceRefresh();
            wxYield();

            wxMessageBox("Error: El valor de IDHab ya está presente en la tabla principal.", "Error de Validación", wxOK | wxICON_WARNING);
            return; // No cerrar ni enviar los datos si ya existe
        }

        // Enviar los valores seleccionados a HabitacionFrame
        habitacionFrame->SetSelectedIDHabValue(idHabValue);       // Coloca el valor de IDHab en la columna 0
        habitacionFrame->SetSelectedDescripcionValue(descripcionValue); // Coloca el valor de Descripcion en la columna 1
        habitacionFrame->Enable();  // Habilita la ventana principal de nuevo
    }

    Close();  // Cierra la ventana de búsqueda después de seleccionar
}

void GridBuscarIDHabFrame::DesactivarEventosGrid() {
    grid->Unbind(wxEVT_GRID_SELECT_CELL, &GridBuscarIDHabFrame::OnGridSelectCell, this);
}

void GridBuscarIDHabFrame::ActivarEventosGrid() {
    grid->Bind(wxEVT_GRID_SELECT_CELL, &GridBuscarIDHabFrame::OnGridSelectCell, this);
}

void GridBuscarIDHabFrame::OnGridSelectCell(wxGridEvent& event) {
    if (IsActive()) {  // Solo actualizar si la ventana está activa
        int row = event.GetRow();
        UpdateRowLabels(row);  // Actualizar etiquetas de las filas
    }
    event.Skip();  // Permitir que otros manejadores sigan funcionando
}

void GridBuscarIDHabFrame::UpdateRowLabels(int selectedRow)
{
    if (!grid || !grid->IsShown()) {
        return;  // Si el grid no existe o no está visible, salir
    }

    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        grid->SetRowLabelValue(row, (row == selectedRow) ? ">" : "");
    }
}

bool GridBuscarIDHabFrame::ConectarBaseDeDatos()
{
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void GridBuscarIDHabFrame::BuscarHabitacionesEnBaseDeDatos(const wxString& idHab, const wxString& descripcion)
{
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = L"SELECT DISTINCT IdHab, Descripcion FROM Habitacion WHERE 1=1";
    if (!idHab.IsEmpty()) {
        query += wxString::Format(L" AND IdHab LIKE '%%%s%%'", idHab);
    }
    if (!descripcion.IsEmpty()) {
        query += wxString::Format(L" AND Descripcion LIKE '%%%s%%'", descripcion);
    }
    query += L" ORDER BY IdHab;";

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int row = 0;
    bool hasResults = false;  // Variable para verificar si hay resultados

    if (ret == SQL_SUCCESS || SQL_SUCCESS_WITH_INFO) {
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            hasResults = true;

            SQLWCHAR idHab[255], descripcion[255];
            SQLGetData(hStmt, 1, SQL_C_WCHAR, idHab, sizeof(idHab), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);

            if (row >= grid->GetNumberRows()) {
                grid->AppendRows(1);
            }

            grid->SetCellValue(row, 0, wxString(idHab));
            grid->SetCellValue(row, 1, wxString(descripcion));

            row++;
        }
    }
    else {
        // Refrescar la interfaz y el grid antes de mostrar el mensaje de error
        grid->ForceRefresh();
        wxYield();

        wxMessageBox("Error al buscar en la base de datos.", "Error", wxOK | wxICON_ERROR);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Si hay menos de 100 filas llenas, añadir filas vacías hasta alcanzar 100
    int emptyRows = 100 - row;
    if (emptyRows > 0) {
        grid->AppendRows(emptyRows);
    }

    // Refrescar el grid para asegurarse de que se vea correctamente
    grid->ForceRefresh();
    wxYield();

    // Mostrar un mensaje de información si no se encontraron resultados
    if (!hasResults) {
        wxMessageBox("No se encontraron coincidencias para los criterios ingresados.", "Información", wxOK | wxICON_INFORMATION);
    }

    // Actualizar las etiquetas de fila después de la búsqueda
    UpdateRowLabels(-1);
}

void GridBuscarIDHabFrame::CerrarConexion()
{
    // Verificar si el handle de la conexión es válido antes de liberar
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);  // Desconectar la base de datos
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);  // Liberar el handle de la conexión
        hDbc = SQL_NULL_HDBC;  // Establecer a NULL después de liberar
    }

    // Verificar si el handle del entorno es válido antes de liberar
    if (hEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);  // Liberar el handle del entorno
        hEnv = SQL_NULL_HENV;  // Establecer a NULL después de liberar
    }
}


