#include "CreacionCategoriaFrame.h"
#include "MainFrame.h"
#include "IDs.h"
#include <wx/grid.h>
#include <sqlext.h>
#include <algorithm>
#include <locale>
#include <unordered_map>
#include <wx/dir.h>

wxBEGIN_EVENT_TABLE(CreacionCategoriaFrame, wxFrame)
EVT_BUTTON(ID_BTN_NUEVO, CreacionCategoriaFrame::OnNuevo)
EVT_BUTTON(ID_BTN_GUARDAR, CreacionCategoriaFrame::OnGuardar)
EVT_BUTTON(ID_BTN_EDITAR, CreacionCategoriaFrame::OnEditar)
EVT_BUTTON(ID_BTN_CANCELAR, CreacionCategoriaFrame::OnCancelar)
EVT_BUTTON(ID_BTN_ELIMINAR, CreacionCategoriaFrame::OnEliminar)
EVT_BUTTON(ID_BTN_BUSCAR, CreacionCategoriaFrame::OnBuscar)
EVT_BUTTON(ID_BTN_IZQUIERDA, CreacionCategoriaFrame::OnIzquierda)
EVT_BUTTON(ID_BTN_DERECHA, CreacionCategoriaFrame::OnDerecha)
EVT_BUTTON(ID_BTN_EXPORTAR, CreacionCategoriaFrame::OnExportarExcel) // Añadido para exportar a Excel
EVT_TEXT(wxID_ANY, CreacionCategoriaFrame::OnDescripcionChange)
EVT_CHAR_HOOK(CreacionCategoriaFrame::OnKeyDown)
EVT_CLOSE(CreacionCategoriaFrame::OnClose)
wxEND_EVENT_TABLE()

CreacionCategoriaFrame::CreacionCategoriaFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuario)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),
    mainFrameParent(parent), usuarioAutenticado(usuario), conexionAbierta(false), isEditing(false), isNewOperation(false), hDbc(NULL)
{
    // Set default font
    defaultFont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Initialize buttons
    btnNuevo = new wxButton(panel, ID_BTN_NUEVO, "Nuevo", wxPoint(10, 10), wxSize(80, 30));
    btnNuevo->SetFont(defaultFont);

    btnGuardar = new wxButton(panel, ID_BTN_GUARDAR, "Guardar", wxPoint(100, 10), wxSize(80, 30));
    btnGuardar->SetFont(defaultFont);
    btnGuardar->Enable(false);

    btnEditar = new wxButton(panel, ID_BTN_EDITAR, "Editar", wxPoint(190, 10), wxSize(80, 30));
    btnEditar->SetFont(defaultFont);

    btnBuscar = new wxButton(panel, ID_BTN_BUSCAR, "Buscar", wxPoint(280, 10), wxSize(80, 30));
    btnBuscar->SetFont(defaultFont);

    btnCancelar = new wxButton(panel, ID_BTN_CANCELAR, "Cancelar", wxPoint(370, 10), wxSize(80, 30));
    btnCancelar->SetFont(defaultFont);
    btnCancelar->Enable(false);

    btnIzquierda = new wxButton(panel, ID_BTN_IZQUIERDA, "<", wxPoint(460, 10), wxSize(40, 30));
    btnIzquierda->SetFont(defaultFont);
    btnIzquierda->Enable(false);

    btnDerecha = new wxButton(panel, ID_BTN_DERECHA, ">", wxPoint(510, 10), wxSize(40, 30));
    btnDerecha->SetFont(defaultFont);
    btnDerecha->Enable(false);

    btnEliminar = new wxButton(panel, ID_BTN_ELIMINAR, "Eliminar", wxPoint(610, 10), wxSize(80, 30));
    btnEliminar->SetFont(defaultFont);

    // Añadir botón "Exportar" en la parte inferior derecha
    btnExportar = new wxButton(panel, ID_BTN_EXPORTAR, "Exportar Excel", wxPoint(570, 580), wxSize(120, 30));  // Añadido para Exportar Excel
    btnExportar->SetFont(defaultFont);
    btnExportar->Enable(true);

    // Initialize text fields
    wxStaticText* lblOperacion = new wxStaticText(panel, wxID_ANY, "Operacion:", wxPoint(10, 50));
    lblOperacion->SetFont(defaultFont);

    txtOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(140, 47), wxSize(50, 23));
    txtOperacion->SetFont(defaultFont);
    txtOperacion->Enable(false);

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 90));
    lblDescripcion->SetFont(defaultFont);

    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(140, 87), wxSize(200, 23));
    txtDescripcion->SetFont(defaultFont);
    txtDescripcion->Enable(false);

    lblSimilitud = new wxStaticText(panel, wxID_ANY, "", wxPoint(355, 91), wxSize(200, 25));
    lblSimilitud->SetFont(defaultFont);

    // Conectar a la base de datos
    ConectarBaseDeDatos();

    // Cargar el último registro si existen registros
    if (HayRegistrosEnTabla()) {
        CargarUltimoRegistro();
        int firstID = ObtenerPrimerOperacion();
        int lastID = ObtenerUltimaOperacion();
        int currentID = wxAtoi(txtOperacion->GetValue());

        if (currentID == firstID) {
            btnIzquierda->Enable(false);
        }
        else {
            btnIzquierda->Enable(true);
        }

        if (currentID == lastID) {
            btnDerecha->Enable(false);
        }
        else {
            btnDerecha->Enable(true);
        }
    }
    else {
        UpdateButtonStates(false, false);
        btnGuardar->Enable(false);
        btnEditar->Enable(false);
        btnEliminar->Enable(false);
        btnBuscar->Enable(false);
        btnCancelar->Enable(false);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
    }
}

CreacionCategoriaFrame::~CreacionCategoriaFrame() {
    CerrarConexion(); // Ensure connection is closed when the object is destroyed
}

    void CreacionCategoriaFrame::OnDescripcionChange(wxCommandEvent& event) {
        if (!txtDescripcion->IsEnabled()) {
            lblSimilitud->SetLabel(""); // Si el campo no está habilitado, no mostrar nada
            return;
        }

        // Guardar la posición actual del cursor
        long posicionCursor = txtDescripcion->GetInsertionPoint();

        // Convertir el valor actual a mayúsculas
        wxString descripcion = txtDescripcion->GetValue();
        descripcion.MakeUpper();  // Convertir a mayúsculas

        // Actualizar el valor sin disparar otro evento
        txtDescripcion->ChangeValue(descripcion);

        // Restaurar la posición del cursor para evitar que salte al inicio
        txtDescripcion->SetInsertionPoint(posicionCursor);

        // Mostrar similitud solo si se está editando activamente y la descripción no es idéntica
        if (!descripcion.IsEmpty()) {
            wxString descripcionSimilar = BuscarDescripcionSimilar(descripcion);

            // Verificar si la descripción es idéntica antes de mostrar similitud
            if (!descripcionSimilar.IsEmpty() && descripcion != descripcionSimilar) {
                lblSimilitud->SetLabel(wxString::Format("Parecido a %s", descripcionSimilar));
            }
            else {
                lblSimilitud->SetLabel(""); // No mostrar si es idéntica o no hay similitud
            }
        }
        else {
            lblSimilitud->SetLabel("");  // Limpiar el mensaje si no hay valor
        }
    }

    wxString CreacionCategoriaFrame::BuscarDescripcionSimilar(const wxString& descripcion) {
        const int UMBRAL_SIMILITUD = 3; // Umbral de similitud para la comparación

        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT Descripcion FROM Categoria;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        wxString descripcionNormalizada = NormalizarCadena(descripcion);
        wxString descripcionMasParecida;
        int distanciaMinima = std::numeric_limits<int>::max();

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                SQLWCHAR descripcionExistente[255];
                SQLGetData(hStmt, 1, SQL_C_WCHAR, descripcionExistente, sizeof(descripcionExistente), NULL);
                wxString descripcionExistenteStr(descripcionExistente);
                wxString descripcionExistenteNormalizada = NormalizarCadena(descripcionExistenteStr);

                int distancia = CalcularDistanciaLevenshtein(descripcionNormalizada, descripcionExistenteNormalizada);

                int longitudMaxima = std::max(descripcionNormalizada.Length(), descripcionExistenteNormalizada.Length());
                if (longitudMaxima > 0) {
                    double similitudRelativa = static_cast<double>(distancia) / longitudMaxima;
                    if (similitudRelativa <= 0.35) {
                        if (distancia < distanciaMinima) {
                            distanciaMinima = distancia;
                            descripcionMasParecida = descripcionExistenteStr;
                        }
                    }
                }
                else if (distancia <= UMBRAL_SIMILITUD) {
                    if (distancia < distanciaMinima) {
                        distanciaMinima = distancia;
                        descripcionMasParecida = descripcionExistenteStr;
                    }
                }
            }
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return descripcionMasParecida;
    }

    wxString CreacionCategoriaFrame::NormalizarCadena(const wxString& cadena) {
        wxString resultado = cadena.Lower(); // Convertir a minúsculas

        // Reemplazar caracteres con tildes y eliminar 'h'
        std::unordered_map<char, char> reemplazos = {
            {'á', 'a'}, {'é', 'e'}, {'í', 'i'}, {'ó', 'o'}, {'ú', 'u'},
            {'Á', 'a'}, {'É', 'e'}, {'Í', 'i'}, {'Ó', 'o'}, {'Ú', 'u'},
            {'ü', 'u'}, {'Ü', 'u'}
        };
        resultado.Replace("h", "");

        wxString cadenaNormalizada;
        for (auto ch : resultado) {
            if (reemplazos.find(ch) != reemplazos.end()) {
                cadenaNormalizada += reemplazos[ch];
            }
            else {
                cadenaNormalizada += ch;
            }
        }
        return cadenaNormalizada;
    }

    bool EsNombreRepetitivo(const wxString& descripcion) {
        if (descripcion.Length() <= 1) {
            return true; // Si la longitud es 1 o menos, lo consideramos repetitivo.
        }

        char primerCaracter = descripcion[0];
        for (size_t i = 1; i < descripcion.Length(); ++i) {
            if (descripcion[i] != primerCaracter) {
                return false; // Si encontramos un caracter diferente, no es repetitivo.
            }
        }
        return true; // Todos los caracteres son iguales.
    }

    int CreacionCategoriaFrame::CalcularDistanciaLevenshtein(const wxString& source, const wxString& target) {
        size_t len1 = source.length(), len2 = target.length();
        std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

        for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
        for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;

        for (size_t i = 1; i <= len1; ++i) {
            for (size_t j = 1; j <= len2; ++j) {
                int cost = (source[i - 1] == target[j - 1]) ? 0 : 1;
                dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
            }
        }

        return dp[len1][len2];
    }

    bool CreacionCategoriaFrame::HayRegistrosEnTabla() {
        SQLHSTMT hStmt = NULL;
        if (hDbc == NULL) {
            wxMessageBox("Conexión a la base de datos no está inicializada.", "Error", wxOK | wxICON_ERROR);
            return false;
        }

        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT COUNT(*) FROM Categoria;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int count = 0;
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return count > 0;
    }

    wxString CreacionCategoriaFrame::BuscarOperacionPorIDHab(const wxString& idHab) {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

        // Consulta para obtener el OperacionID basado en el IDHab
        wxString query = wxString::Format(L"SELECT Operacion FROM Habitacion WHERE IdHab = '%s';", idHab);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        wxString operacionID;
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            if (SQLFetch(hStmt) == SQL_SUCCESS) {
                SQLWCHAR operacion[20];
                SQLGetData(hStmt, 1, SQL_C_WCHAR, operacion, sizeof(operacion), NULL);
                operacionID = wxString(operacion);
            }
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        return operacionID;  // Retornar el OperacionID encontrado o vacío si no se encuentra
    }

    void CreacionCategoriaFrame::OnNuevo(wxCommandEvent& event) {
        EnableInputs(true);
        ClearInputs();
        lblSimilitud->SetLabel("");  // Limpiar mensaje de similitud
        isNewOperation = true;
        isEditing = false;

        // Obtener el siguiente número de operación
        int siguienteOperacion = ObtenerSiguienteOperacion();
        txtOperacion->SetValue(wxString::Format("%d", siguienteOperacion));

        UpdateButtonStates(true, false);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
    }

    bool CreacionCategoriaFrame::OperacionExists(int operacion) {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = wxString::Format(L"SELECT COUNT(*) FROM Categoria WHERE Operacion = %d;", operacion);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int count = 0;
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return count > 0;  // Retorna true si la operación ya existe
    }

    void CreacionCategoriaFrame::OnGuardar(wxCommandEvent& event) {
        lblSimilitud->SetLabel("");

        // Verificación de campos obligatorios
        if (!ValidateInputs()) {
            wxMessageBox("Todos los campos son obligatorios.", "Error de Validación", wxOK | wxICON_WARNING);
            return;
        }

        wxString descripcion = txtDescripcion->GetValue();

        // Verificación de nombre repetitivo
        if (EsNombreRepetitivo(descripcion)) {
            wxMessageBox("El nombre ingresado no puede consistir únicamente de caracteres repetidos.", "Error de Validación", wxOK | wxICON_WARNING);
            return;
        }

        long operacionValue;

        // Verificar si el campo 'Operacion' tiene un valor numérico
        if (!txtOperacion->GetValue().ToLong(&operacionValue)) {
            wxMessageBox("El campo 'Operacion' solo admite valores numéricos.", "Error de Validación", wxOK | wxICON_WARNING);
            return;
        }

        // Verificar si la operación ya existe en la base de datos
        if (!isEditing && OperacionExists(operacionValue)) {
            wxMessageBox("El número de operación ya existe. Se generará uno nuevo.", "Número de Operación Duplicado", wxOK | wxICON_WARNING);
            operacionValue = ObtenerSiguienteOperacion();  // Generar nuevo número de operación
            txtOperacion->SetValue(wxString::Format("%d", operacionValue));  // Actualizar el campo de Operacion
            return;  // Detener aquí para que el usuario revise el nuevo número de operación
        }

        // Verificar conexión a la base de datos
        if (!conexionAbierta) {
            wxMessageBox("Error: No hay conexión con la base de datos.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        // Obtener el usuario actual y el nombre del equipo
        wxString usuarioID = usuarioAutenticado;
        wxString ordenador = wxGetHostName();

        SQLHSTMT hStmt = NULL;
        SQLRETURN ret;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

        wxString query;
        if (isEditing) {
            query = L"UPDATE Categoria SET Descripcion = ?, UsuarioID = ?, Ordenador = ? WHERE Operacion = ?;";
        }
        else {
            query = L"INSERT INTO Categoria (Operacion, Descripcion, UsuarioID, Ordenador) VALUES (?, ?, ?, ?);";
        }

        ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int operacion = wxAtoi(txtOperacion->GetValue());

        // Bindeo de parámetros
        if (isEditing) {
            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacion, 0, NULL);
        }
        else {
            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacion, 0, NULL);
            SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
        }

        // Ejecutar la consulta
        ret = SQLExecute(hStmt);

        // Manejo de errores durante la ejecución
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            SQLWCHAR sqlState[6], messageText[256];
            SQLINTEGER nativeError;
            SQLSMALLINT textLength;
            SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);

            if (wxString(sqlState) == "23000") {  // Código de error para violación de clave única
                wxMessageBox("El nombre ingresado ya existe en la base de datos.", "Error de Validación", wxOK | wxICON_WARNING);
            }
            else {
                wxString errorMsg = wxString::Format("Error al guardar los datos.\nSQL State: %s\nError: %d\n%s",
                    wxString(sqlState), nativeError, wxString(messageText));
                wxMessageBox(errorMsg, "Error", wxOK | wxICON_ERROR);
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        // Deshabilitar campos y actualizar botones tras guardado
        EnableInputs(false);
        UpdateButtonStates(false, false);

        // *** Aquí actualizamos las banderas, ya que los cambios fueron guardados ***
        isEditing = false;
        isNewOperation = false;  // Dejar de considerar la operación como "nueva"

        // Verificar si la operación guardada es la primera o la última
        int operacionActual = wxAtoi(txtOperacion->GetValue());
        if (operacionActual == ObtenerPrimerOperacion()) {
            btnIzquierda->Enable(false);  // Bloquear botón izquierda si es la primera operación
        }
        else {
            btnIzquierda->Enable(true);  // Desbloquear si no es la primera
        }

        if (operacionActual == ObtenerUltimaOperacion()) {
            btnDerecha->Enable(false);  // Bloquear botón derecha si es la última operación
        }
        else {
            btnDerecha->Enable(true);  // Desbloquear si no es la última
        }
    }

    void CreacionCategoriaFrame::OnEditar(wxCommandEvent& event) {
        originalOperacion = wxAtoi(txtOperacion->GetValue());
        originalDescripcion = txtDescripcion->GetValue();

        EnableInputs(true);
        lblSimilitud->SetLabel("");  // Limpiar mensaje de similitud al entrar en modo edición
        UpdateButtonStates(false, true);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
        isEditing = true;
    }

    void CreacionCategoriaFrame::OnBuscar(wxCommandEvent& event) {
        lblSimilitud->SetLabel("");

        // Usar la nueva clase BuscarFrameCategoria para realizar la búsqueda
        BuscarFrameCategoria* buscarFrame = new BuscarFrameCategoria(this, "Buscar Categorías", wxDefaultPosition, wxSize(750, 500));
        buscarFrame->CentreOnParent();
        buscarFrame->Show(true);
        Disable(); // Desactiva el frame actual mientras se muestra la ventana de búsqueda
    }

    void CreacionCategoriaFrame::BuscarOperacionCategoria(wxString operacionID, wxString descripcion) {
        if (!conexionAbierta) {
            wxMessageBox("Error: No hay conexión con la base de datos.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        // Consulta para buscar la Operación por su ID en la tabla de Categoria
        wxString query = wxString::Format(L"SELECT Operacion, Descripcion FROM Categoria WHERE Operacion = '%s';", operacionID);
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        wxString fetchedOperacionID, fetchedDescripcion;
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            if (SQLFetch(hStmt) == SQL_SUCCESS) {
                SQLWCHAR operacion[20], descripcion[255];
                SQLGetData(hStmt, 1, SQL_C_WCHAR, operacion, sizeof(operacion), NULL);    // Obtener Operacion
                SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL); // Obtener Descripcion
                fetchedOperacionID = wxString(operacion);
                fetchedDescripcion = wxString(descripcion);
            }
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        if (!fetchedOperacionID.IsEmpty()) {
            txtOperacion->SetValue(fetchedOperacionID);  // Mostrar la OperacionID obtenida
            txtDescripcion->SetValue(fetchedDescripcion); // Mostrar la Descripción obtenida
        }
        else {
            wxMessageBox("No se encontró una Operacion en la categoría.", "Error", wxOK | wxICON_ERROR);
        }
    }

    void CreacionCategoriaFrame::OnCancelar(wxCommandEvent& event) {
        // Deshabilitar temporalmente el manejador de eventos para evitar ciclos.
        this->Unbind(wxEVT_CLOSE_WINDOW, &CreacionCategoriaFrame::OnClose, this);

        lblSimilitud->SetLabel("");  // Limpiar mensaje de similitud al cancelar

        if (isNewOperation) {
            int operacion = wxAtoi(txtOperacion->GetValue());
            if (operacion == 0 || !HayRegistrosEnTabla()) {
                ClearInputs();
                EnableInputs(false);
                UpdateButtonStates(false, false);
                btnEditar->Enable(false);
                btnIzquierda->Enable(false);
                btnDerecha->Enable(false);
                btnBuscar->Enable(false);
                btnEliminar->Enable(false);
                isNewOperation = false;
            }
            else {
                CargarUltimoRegistro();
                isNewOperation = false;
            }
        }
        else if (isEditing) {
            txtOperacion->SetValue(wxString::Format("%d", originalOperacion));
            txtDescripcion->SetValue(originalDescripcion);
            isEditing = false;
        }

        if (!HayRegistrosEnTabla()) {
            ClearInputs();
            EnableInputs(false);
            UpdateButtonStates(false, false);
            btnNuevo->Enable(true);
            btnGuardar->Enable(false);
            btnEditar->Enable(false);
            btnEliminar->Enable(false);
            btnBuscar->Enable(false);
            btnCancelar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            return;
        }

        EnableInputs(false);
        UpdateButtonStates(false, false);

        // Verificar si la operación actual es la primera o la última después de cancelar
        int operacionActual = wxAtoi(txtOperacion->GetValue());
        if (operacionActual == ObtenerPrimerOperacion()) {
            btnIzquierda->Enable(false);  // Bloquear botón izquierda si es la primera operación
        }
        else {
            btnIzquierda->Enable(true);  // Desbloquear si no es la primera
        }

        if (operacionActual == ObtenerUltimaOperacion()) {
            btnDerecha->Enable(false);  // Bloquear botón derecha si es la última operación
        }
        else {
            btnDerecha->Enable(true);  // Desbloquear si no es la última
        }

        // Reactivar el manejador de eventos después de cancelar.
        this->Bind(wxEVT_CLOSE_WINDOW, &CreacionCategoriaFrame::OnClose, this);
    }

    void CreacionCategoriaFrame::ActualizarEstadoBotonesNavegacionCategoria() {
        int operacionActual = wxAtoi(txtOperacion->GetValue());

        // Verificar si es la primera operación
        if (operacionActual == ObtenerPrimerOperacion()) {
            btnIzquierda->Enable(false);  // Bloquear botón izquierda si es la primera operación
        }
        else {
            btnIzquierda->Enable(true);  // Desbloquear si no es la primera
        }

        // Verificar si es la última operación
        if (operacionActual == ObtenerUltimaOperacion()) {
            btnDerecha->Enable(false);  // Bloquear botón derecha si es la última operación
        }
        else {
            btnDerecha->Enable(true);  // Desbloquear si no es la última
        }
    }

    int CreacionCategoriaFrame::ObtenerSiguienteOperacion() {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT ISNULL(MAX(Operacion), 0) + 1 FROM Categoria;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int siguienteOperacion = 1;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacion, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return siguienteOperacion;
    }

    void CreacionCategoriaFrame::CargarUltimoRegistro() {
        int ultimoID = ObtenerUltimaOperacion();
        if (ultimoID != -1) {
            CargarDatosOperacion(ultimoID);

            if (ObtenerPrimerOperacion() == ultimoID && ObtenerUltimaOperacion() == ultimoID) {
                btnIzquierda->Enable(false);
                btnDerecha->Enable(false);
            }
            else {
                btnDerecha->Enable(false);
                btnIzquierda->Enable(true);
            }
        }
    }

    int CreacionCategoriaFrame::ObtenerPrimerOperacion() {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT MIN(Operacion) FROM Categoria;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int firstID = -1;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &firstID, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        return firstID;
    }

    int CreacionCategoriaFrame::ObtenerUltimaOperacion() {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT MAX(Operacion) FROM Categoria;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int lastID = -1;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &lastID, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        return lastID;
    }

    void CreacionCategoriaFrame::OnIzquierda(wxCommandEvent& event) {
        int currentID = wxAtoi(txtOperacion->GetValue());
        int previousID = ObtenerOperacionAnterior(currentID);

        if (previousID != -1) {
            CargarDatosOperacion(previousID);
        }
        else {
            btnIzquierda->Enable(false);
        }
    }

    void CreacionCategoriaFrame::OnDerecha(wxCommandEvent& event) {
        int currentID = wxAtoi(txtOperacion->GetValue());
        int nextID = ObtenerOperacionSiguiente(currentID);

        if (nextID != -1) {
            CargarDatosOperacion(nextID);
        }
        else {
            btnDerecha->Enable(false);
        }
    }

    int CreacionCategoriaFrame::ObtenerOperacionAnterior(int currentID) {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = wxString::Format(L"SELECT MAX(Operacion) FROM Categoria WHERE Operacion < %d;", currentID);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int previousID = -1;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &previousID, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return previousID;
    }

    int CreacionCategoriaFrame::ObtenerOperacionSiguiente(int currentID) {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = wxString::Format(L"SELECT MIN(Operacion) FROM Categoria WHERE Operacion > %d;", currentID);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        int nextID = -1;

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            SQLFetch(hStmt);
            SQLGetData(hStmt, 1, SQL_C_LONG, &nextID, 0, NULL);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return nextID;
    }

    void CreacionCategoriaFrame::OnEliminar(wxCommandEvent& event) {

        lblSimilitud->SetLabel("");

        if (!conexionAbierta && !ConectarBaseDeDatos()) {
            wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        int operacion = wxAtoi(txtOperacion->GetValue());
        if (operacion == 0) {
            wxMessageBox("No hay una Operacion válida para eliminar.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        int respuesta = wxMessageBox("¿Está seguro de que desea eliminar este registro?", "Confirmar eliminación", wxYES_NO | wxICON_QUESTION);
        if (respuesta == wxNO) {
            return;
        }

        // Deshabilitar la ventana durante la operación de eliminación
        this->Disable();

        SQLHSTMT hStmt = NULL;
        SQLRETURN ret;
        bool error = false;

        try {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
            wxString query = L"DELETE FROM Categoria WHERE Operacion = ?;";
            ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                wxMessageBox("Error al preparar la consulta para eliminar.", "Error", wxOK | wxICON_ERROR);
                error = true;
                throw std::runtime_error("Error preparing SQL statement for deletion.");
            }

            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacion, 0, NULL);
            ret = SQLExecute(hStmt);

            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                wxMessageBox("Error al eliminar el registro.", "Error", wxOK | wxICON_ERROR);
                error = true;
                throw std::runtime_error("Error executing SQL statement for deletion.");
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

            // Verificar si la tabla quedó vacía tras la eliminación
            if (!HayRegistrosEnTabla()) {
                ClearInputs();
                EnableInputs(false);
                UpdateButtonStates(false, false);
                btnNuevo->Enable(true);
                btnGuardar->Enable(false);
                btnEditar->Enable(false);
                btnEliminar->Enable(false);
                btnBuscar->Enable(false);
                btnCancelar->Enable(false);
                btnIzquierda->Enable(false);
                btnDerecha->Enable(false);
                this->Enable();  // Rehabilitar la ventana
                return;
            }

            // Cargar el siguiente registro o el anterior si no hay más
            int nextID = ObtenerOperacionSiguiente(operacion);
            if (nextID != -1) {
                CargarDatosOperacion(nextID);
            }
            else {
                int previousID = ObtenerOperacionAnterior(operacion);
                if (previousID != -1) {
                    CargarDatosOperacion(previousID);
                }
                else {
                    ClearInputs();
                }
            }

        }
        catch (const std::exception& ex) {
            if (hStmt != NULL) {
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            }
            wxMessageBox(wxString::Format("Se produjo un error durante la eliminación: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
            error = true;
        }

        // Rehabilitar la ventana independientemente de si hubo un error o no
        this->Enable();
    }

    void CreacionCategoriaFrame::OnClose(wxCloseEvent& event) {
        // Solo preguntar si está en modo edición o modo nuevo
        if (isEditing || isNewOperation) {
            int respuesta = wxMessageBox("Hay cambios no guardados. ¿Seguro que desea cerrar?", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
            if (respuesta == wxYES) {
                if (mainFrameParent != nullptr) {
                    mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
                }
                event.Skip();  // Permitir el cierre
            }
            else {
                event.Veto();  // Cancelar el cierre si el usuario rechaza
            }
        }
        else {
            // Si no está en modo edición o nuevo, cerrar la ventana directamente
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
            }
            event.Skip();  // Permitir el cierre directamente
        }
    }

    void CreacionCategoriaFrame::OnKeyDown(wxKeyEvent& event) {
        if (event.GetKeyCode() == WXK_ESCAPE) {
            // Solo preguntar si está en modo edición o modo nuevo
            if (isEditing || isNewOperation) {
                int respuesta = wxMessageBox("Hay cambios sin guardar.¿Desea cancelar los cambios? ", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
                if (respuesta == wxYES) {
                    wxCommandEvent evt;  // Crear un evento wxCommandEvent vacío
                    OnCancelar(evt);     // Pasar el evento vacío
                }
                else {
                    return;  // No cerrar, el usuario canceló
                }
            }
            else {
                // Si no está en modo edición o nuevo, cerrar la ventana directamente
                if (mainFrameParent != nullptr) {
                    mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
                }
                Destroy();  // Elimina la ventana directamente
            }
        }
        else {
            event.Skip();  // Permitir que otros eventos de teclado se manejen
        }
    }

    void CreacionCategoriaFrame::EnableInputs(bool enable) {
        txtOperacion->Enable(false);  // Aseguramos que txtOperacion esté siempre bloqueado
        txtDescripcion->Enable(enable); // Solo la descripción puede habilitarse

        if (!enable) {
            lblSimilitud->SetLabel("");  // Limpiar mensaje de similitud cuando se deshabilita el campo de entrada
        }
    }

    void CreacionCategoriaFrame::ClearInputs() {
        txtOperacion->Clear();
        txtDescripcion->Clear();
    }

    void CreacionCategoriaFrame::UpdateButtonStates(bool nuevoSelected, bool editarSelected) {
        btnNuevo->Enable(!nuevoSelected && !editarSelected);
        btnBuscar->Enable(!nuevoSelected && !editarSelected);
        btnEliminar->Enable(!nuevoSelected && !editarSelected);
        btnGuardar->Enable(nuevoSelected || editarSelected);
        btnCancelar->Enable(nuevoSelected || editarSelected);
        btnEditar->Enable(!nuevoSelected && !editarSelected);
        btnExportar->Enable(!nuevoSelected && !editarSelected);
    }

    bool CreacionCategoriaFrame::ConectarBaseDeDatos() {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
        wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
        SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

        conexionAbierta = (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
        return conexionAbierta;
    }

    void CreacionCategoriaFrame::OnExportarExcel(wxCommandEvent& event) {
        try {
            // Obtener la fecha y hora actual
            wxDateTime now = wxDateTime::Now();
            wxString datePart = now.Format("%d%m%Y");  // Formato: DDMMYYYY
            wxString timePart = now.Format("%I%M%S%p"); // Formato: HHMMSSAM/PM

            // Ruta base donde se almacenarán los archivos
            wxString basePath = "D:\\RSC Sistema\\temprsc";

            // Comprobar si la carpeta "temprsc" existe, si no, crearla
            if (!wxDirExists(basePath)) {
                wxMkdir(basePath);
            }

            // Comprobar si ya existe una carpeta con la fecha de hoy
            wxString folderPath = basePath + "\\" + datePart + timePart;

            // Verificar si existe una carpeta para la fecha actual
            bool carpetaExiste = false;
            wxDir dir(basePath);
            wxString subdir;
            bool continuar = dir.GetFirst(&subdir);
            while (continuar) {
                if (subdir.StartsWith(datePart)) {
                    carpetaExiste = true;
                    folderPath = basePath + "\\" + subdir;  // Usar la carpeta existente
                    break;
                }
                continuar = dir.GetNext(&subdir);
            }

            // Si no existe la carpeta para la fecha actual, crearla
            if (!carpetaExiste) {
                wxMkdir(folderPath);
            }

            // Crear el nombre completo del archivo de salida
            wxString exportPath = folderPath + "\\categoria.xlsx";  // Cambiado el nombre a categoria.xlsx

            // Cargar el archivo de plantilla de Excel
            xlnt::workbook wb;
            wb.load("D:\\RSC Sistema\\exportar_atributos.xlsx");  // Cargar la plantilla de Excel

            // Acceder a la primera hoja
            xlnt::worksheet ws = wb.active_sheet();

            // Consulta SQL para obtener los datos de la tabla Categoria
            SQLHSTMT hStmt = NULL;
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
            wxString query = L"SELECT Operacion, Descripcion FROM Categoria;";
            SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                int row = 2;  // Empezamos en la fila 2 ya que la primera es para encabezados

                // Procesar cada registro
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    int operacion;
                    SQLWCHAR descripcion[255];

                    SQLGetData(hStmt, 1, SQL_C_LONG, &operacion, 0, NULL);  // Obtener Operacion
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);  // Obtener Descripcion

                    // Escribir en las celdas de Excel
                    ws.cell(xlnt::cell_reference(1, row)).value(operacion);  // Columna A para Operacion
                    ws.cell(xlnt::cell_reference(2, row)).value(wxString(descripcion).ToStdString());  // Columna B para Descripcion

                    row++;  // Avanzar a la siguiente fila
                }
            }

            // Liberar el handle del SQL
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

            // Guardar el archivo Excel en la ruta generada
            wb.save(exportPath.ToStdString());

            // Abrir automáticamente el archivo exportado
            ShellExecute(NULL, L"open", exportPath.wc_str(), NULL, NULL, SW_SHOW);
        }
        catch (const std::exception& ex) {
            wxMessageBox(wxString::Format("Error al exportar Excel: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
        }
    }

    void CreacionCategoriaFrame::CargarDatosOperacion(int operacion) {
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = wxString::Format(L"SELECT Operacion, Descripcion, UsuarioID, Ordenador FROM Categoria WHERE Operacion = %d;", operacion);
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            if (SQLFetch(hStmt) == SQL_SUCCESS) {
                int fetchedOperacion;
                SQLWCHAR descripcion[255], usuarioID[50], ordenador[250];

                SQLGetData(hStmt, 1, SQL_C_LONG, &fetchedOperacion, 0, NULL);
                SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
                SQLGetData(hStmt, 3, SQL_C_WCHAR, usuarioID, sizeof(usuarioID), NULL);
                SQLGetData(hStmt, 4, SQL_C_WCHAR, ordenador, sizeof(ordenador), NULL);

                txtOperacion->SetValue(wxString::Format("%d", fetchedOperacion));
                txtDescripcion->SetValue(wxString(descripcion));

                int firstID = ObtenerPrimerOperacion();
                int lastID = ObtenerUltimaOperacion();

                if (fetchedOperacion == firstID) {
                    btnIzquierda->Enable(false);
                }
                else {
                    btnIzquierda->Enable(true);
                }

                if (fetchedOperacion == lastID) {
                    btnDerecha->Enable(false);
                }
                else {
                    btnDerecha->Enable(true);
                }
            }
        }
        else {
            wxMessageBox("Error al cargar los datos de la Operacion.", "Error", wxOK | wxICON_ERROR);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }

    bool CreacionCategoriaFrame::ValidateInputs() {
        return !txtDescripcion->GetValue().IsEmpty();
    }

    void CreacionCategoriaFrame::CerrarConexion() {
        if (conexionAbierta) {
            SQLDisconnect(hDbc);
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
            SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
            conexionAbierta = false;
        }
    }