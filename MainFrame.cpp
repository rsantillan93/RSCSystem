#include "MainFrame.h"
#include "CreacionHabitacionesFrame.h"
#include "CreacionCategoriaFrame.h"
#include "CreacionProductoFrame.h"
#include "CreacionIdentificadorFrame.h"
#include "CreacionPresentacionFrame.h"
#include "HabitacionFrame.h"
#include "IDs.h"
#include <wx/wx.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(ID_MENU_HABITACION, MainFrame::OnMenuHabitacion)
EVT_MENU(ID_MENU_CLIENTES, MainFrame::OnMenuClientes)
EVT_MENU(ID_MENU_CATEGORIA, MainFrame::OnMenuCategoria)
EVT_MENU(ID_MENU_PRODUCTO, MainFrame::OnMenuProducto)
EVT_MENU(ID_MENU_IDENTIFICADOR, MainFrame::OnMenuIdentificador)
EVT_MENU(ID_MENU_PRESENTACION, MainFrame::OnMenuPresentacion)
EVT_MENU(ID_MENU_ARTICULO, MainFrame::OnMenuArticulo)
EVT_MENU(ID_MENU_ALQUILER, MainFrame::OnMenuAlquiler)
EVT_MENU(ID_MENU_COCHERA, MainFrame::OnMenuCochera)
EVT_MENU(ID_MENU_ABARROTES, MainFrame::OnMenuAbarrotes)
EVT_MENU(ID_MENU_PRECIO_HABITACION, MainFrame::OnMenuPrecioHabitacion)
EVT_MENU(ID_MENU_PRECIO_ARTICULO, MainFrame::OnMenuPrecioArticulo)
EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

// Constructor del MainFrame que recibe el nombre del usuario autenticado
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& usuario)
    : wxFrame(NULL, wxID_ANY, title, pos, size), usuarioAutenticado(usuario)  // Guarda el usuario
{
    defaultFont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    Maximize(true);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->SetFont(defaultFont);

    wxMenu* menuMaestro = new wxMenu;
    menuMaestro->Append(ID_MENU_HABITACION, "Habitaci�n");
    menuMaestro->Append(ID_MENU_CLIENTES, "Clientes");
    menuMaestro->Append(ID_MENU_ARTICULO, "Art�culo");

    wxMenu* subMenuAtributos = new wxMenu;
    subMenuAtributos->Append(ID_MENU_CATEGORIA, "Categor�a");
    subMenuAtributos->Append(ID_MENU_PRODUCTO, "Producto");
    subMenuAtributos->Append(ID_MENU_IDENTIFICADOR, "Identificador");
    subMenuAtributos->Append(ID_MENU_PRESENTACION, "Presentaci�n");

    wxMenu* subMenuPrecio = new wxMenu;
    subMenuPrecio->Append(ID_MENU_PRECIO_HABITACION, "Precio Habitaci�n");
    subMenuPrecio->Append(ID_MENU_PRECIO_ARTICULO, "Precio Art�culo");

    menuMaestro->AppendSubMenu(subMenuAtributos, "Atributos");
    menuMaestro->AppendSubMenu(subMenuPrecio, "Precio");

    wxMenu* menuServicios = new wxMenu;
    menuServicios->Append(ID_MENU_ALQUILER, "Alquiler");
    menuServicios->Append(ID_MENU_COCHERA, "Cochera");
    menuServicios->Append(ID_MENU_ABARROTES, "Abarrotes");

    menuBar->Append(menuMaestro, "Maestro");
    menuBar->Append(menuServicios, "Servicios");

    SetMenuBar(menuBar);

    CreateStatusBar();
    // Muestra el mensaje de bienvenida con el nombre del usuario autenticado
    SetStatusText(wxString::Format("Bienvenido, %s", usuarioAutenticado));
}

void MainFrame::SetMenuFont(wxMenu* menu)
{
    for (size_t i = 0; i < menu->GetMenuItemCount(); ++i) {
        wxMenuItem* item = menu->FindItemByPosition(i);
        if (item) {
            item->SetFont(defaultFont);
        }
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    int answer = wxMessageBox("�Desea cerrar el sistema?", "Confirmar Cierre", wxYES_NO | wxICON_QUESTION);
    if (answer == wxYES) {
        Destroy();
    }
    else {
        event.Veto();
    }
}

void MainFrame::OnMenuHabitacion(wxCommandEvent& event) {
    CreacionHabitacionesFrame* creacionHabitacionesFrame = new CreacionHabitacionesFrame(
        "Creaci�n de Habitaciones", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    creacionHabitacionesFrame->Centre();
    creacionHabitacionesFrame->Show(true);
    this->Disable();
}
void MainFrame::OnMenuClientes(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Clientes seleccionada", "Gesti�n de Clientes", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuCategoria(wxCommandEvent& event) {
    // Crear y mostrar el frame de Creaci�n de Categor�as
    CreacionCategoriaFrame* creacionCategoriaFrame = new CreacionCategoriaFrame(
        "Creaci�n de Categor�as", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    creacionCategoriaFrame->Centre();
    creacionCategoriaFrame->Show(true);
    this->Disable();
}

void MainFrame::OnMenuProducto(wxCommandEvent& event) {
    // Crear y mostrar el frame de Creaci�n de Categor�as
    CreacionProductoFrame* creacionProductoFrame = new CreacionProductoFrame(
        "Creaci�n de Producto", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    creacionProductoFrame->Centre();
    creacionProductoFrame->Show(true);
    this->Disable();
}
void MainFrame::OnMenuIdentificador(wxCommandEvent& event) {
    // Crear y mostrar el frame de Creaci�n de Categor�as
    CreacionIdentificadorFrame* creacionIdentificadorFrame = new CreacionIdentificadorFrame(
        "Creaci�n de Identificador", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    creacionIdentificadorFrame->Centre();
    creacionIdentificadorFrame->Show(true);
    this->Disable();
}

void MainFrame::OnMenuPresentacion(wxCommandEvent& event) {
    // Crear y mostrar el frame de Creaci�n de Categor�as
    CreacionPresentacionFrame* creacionPresentacionFrame = new CreacionPresentacionFrame(
        "Creaci�n de Presentacion", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    creacionPresentacionFrame->Centre();
    creacionPresentacionFrame->Show(true);
    this->Disable();
}

void MainFrame::OnMenuArticulo(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Art�culos seleccionada", "Gesti�n de Art�culos", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuAlquiler(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Alquileres seleccionada", "Gesti�n de Alquileres", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuCochera(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Cocheras seleccionada", "Gesti�n de Cocheras", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuAbarrotes(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Abarrotes seleccionada", "Gesti�n de Abarrotes", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuPrecioHabitacion(wxCommandEvent& event) {
    HabitacionFrame* habitacionFrame = new HabitacionFrame(
        "Precio de Habitaciones", wxDefaultPosition, wxSize(730, 660), this, usuarioAutenticado  // Pasar el usuario autenticado
    );
    habitacionFrame->Centre();
    habitacionFrame->Show(true);
    this->Disable();
}

void MainFrame::OnMenuPrecioArticulo(wxCommandEvent& event) {
    wxMessageBox("Gesti�n de Precio Art�culo seleccionada", "Gesti�n de Precio Art�culo", wxOK | wxICON_INFORMATION);
}
