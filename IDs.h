// IDs.h
#pragma once

#include <wx/wx.h>  // Incluye el encabezado de wxWidgets para definir wxID_HIGHEST

// Enumeración para identificadores de botones y otros componentes de la interfaz de usuario
enum
{
    // Identificadores de botones principales
    ID_BTN_NUEVO = wxID_HIGHEST + 1,  // Botón "Nuevo"
    ID_BTN_GUARDAR,                   // Botón "Guardar"
    ID_BTN_EDITAR,                    // Botón "Editar"
    ID_BTN_CANCELAR,                  // Botón "Cancelar"
    ID_BTN_ELIMINAR,                  // Botón "Eliminar"
    ID_BTN_BUSCAR,                    // Botón "Buscar"

    // Identificadores de botones adicionales
    ID_BTN_IMPORTAR,                  // Botón "Importar"
    ID_BTN_EXPORTAR,                  // Botón "Exportar"
    ID_BTN_INSERTAR,                  // Botón "Insertar"
    ID_BTN_BORRAR,                    // Botón "Borrar"

    // Identificadores de botones de navegación
    ID_BTN_IZQUIERDA,                 // Botón de navegación a la izquierda
    ID_BTN_DERECHA,                   // Botón de navegación a la derecha

    // Identificadores de elementos de menú
    ID_MENU_HABITACION,               // Menú "Habitación"
    ID_MENU_CLIENTES,                 // Menú "Clientes"
    ID_MENU_CATEGORIA,                // Menú "Categoría"
    ID_MENU_PRODUCTO,                 // Menú "Producto"
    ID_MENU_IDENTIFICADOR,            // Menú "Identificador"
    ID_MENU_PRESENTACION,             // Menú "Presentación"
    ID_MENU_ARTICULO,                 // Menú "Artículo"
    ID_MENU_ALQUILER,                 // Menú "Alquiler"
    ID_MENU_COCHERA,                  // Menú "Cochera"
    ID_MENU_ABARROTES,                // Menú "Abarrotes"
    ID_MENU_PRECIO_HABITACION,        // Menú "Precio Habitación"
    ID_MENU_PRECIO_ARTICULO           // Menú "Precio Artículo"
};
