// MainFrame.h
#pragma once
#include <wx/wx.h>
#include "HabitacionFrame.h"
#include "IDs.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& usuario);

    wxString GetUsuarioAutenticado() const { return usuarioAutenticado; }


private:
    // Eventos de menú
    void OnClose(wxCloseEvent& event);
    void OnMenuHabitacion(wxCommandEvent& event);
    void OnMenuClientes(wxCommandEvent& event);
    void OnMenuCategoria(wxCommandEvent& event);
    void OnMenuProducto(wxCommandEvent& event);
    void OnMenuIdentificador(wxCommandEvent& event);
    void OnMenuPresentacion(wxCommandEvent& event);
    void OnMenuArticulo(wxCommandEvent& event);
    void OnMenuAlquiler(wxCommandEvent& event);
    void OnMenuCochera(wxCommandEvent& event);
    void OnMenuAbarrotes(wxCommandEvent& event);
    void OnMenuPrecioHabitacion(wxCommandEvent& event);
    void OnMenuPrecioArticulo(wxCommandEvent& event);

    // Métodos auxiliares
    void SetMenuFont(wxMenu* menu);
    wxString usuarioAutenticado;
    wxFont defaultFont;

    wxDECLARE_EVENT_TABLE();
};