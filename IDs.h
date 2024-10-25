// IDs.h
#pragma once

#include <wx/wx.h>  // Incluye el encabezado de wxWidgets para definir wxID_HIGHEST

// Enumeraci�n para identificadores de botones y otros componentes de la interfaz de usuario
enum
{
    // Identificadores de botones principales
    ID_BTN_NUEVO = wxID_HIGHEST + 1,  // Bot�n "Nuevo"
    ID_BTN_GUARDAR,                   // Bot�n "Guardar"
    ID_BTN_EDITAR,                    // Bot�n "Editar"
    ID_BTN_CANCELAR,                  // Bot�n "Cancelar"
    ID_BTN_ELIMINAR,                  // Bot�n "Eliminar"
    ID_BTN_BUSCAR,                    // Bot�n "Buscar"

    // Identificadores de botones adicionales
    ID_BTN_IMPORTAR,                  // Bot�n "Importar"
    ID_BTN_EXPORTAR,                  // Bot�n "Exportar"
    ID_BTN_INSERTAR,                  // Bot�n "Insertar"
    ID_BTN_BORRAR,                    // Bot�n "Borrar"

    // Identificadores de botones de navegaci�n
    ID_BTN_IZQUIERDA,                 // Bot�n de navegaci�n a la izquierda
    ID_BTN_DERECHA,                   // Bot�n de navegaci�n a la derecha

    // Identificadores de elementos de men�
    ID_MENU_HABITACION,               // Men� "Habitaci�n"
    ID_MENU_CLIENTES,                 // Men� "Clientes"
    ID_MENU_CATEGORIA,                // Men� "Categor�a"
    ID_MENU_PRODUCTO,                 // Men� "Producto"
    ID_MENU_IDENTIFICADOR,            // Men� "Identificador"
    ID_MENU_PRESENTACION,             // Men� "Presentaci�n"
    ID_MENU_ARTICULO,                 // Men� "Art�culo"
    ID_MENU_ALQUILER,                 // Men� "Alquiler"
    ID_MENU_COCHERA,                  // Men� "Cochera"
    ID_MENU_ABARROTES,                // Men� "Abarrotes"
    ID_MENU_PRECIO_HABITACION,        // Men� "Precio Habitaci�n"
    ID_MENU_PRECIO_ARTICULO           // Men� "Precio Art�culo"
};
