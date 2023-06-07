// Proyecto Derco 		07/06/2023
// Integrantes grupo: Felipe Engels, Gabriel Lopez, Benjamin Palamara 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SistemaDerco{
    char *ceo;
    int cantEmpleados;
    char *rutEmpresarial;
    struct NodoClientes *clientes;
    struct NodoRegion *regiones;
};

struct Region{
    char *nombre;
    int id;
    int cantSedes;
    struct NodoSedes *sedes;
};

struct NodoRegion{
    struct Region *datosRegion;
    struct NodoRegion *sig;                 //lista circular simplemente enlazada
};

struct Sede{
    int idSede;
    char *direccion;
    char *supervisor;
    struct NodoProductos *stockProductos;
};

struct NodoSedes{
    struct Sede *datosSede;
    struct NodoSedes *ant, *sig;            //lista doble enlazada
};

struct Cliente{
    char *nombre;
    char *rut;
    int idCliente;
    struct NodoCompras *listaCompras;
};

struct NodoClientes{
    struct Cliente *datosCliente;
    struct NodoClientes *sig;               //lista circular simplemente enlazada
};

struct Compra{
    int idBoleta;
    int montoTotal;
    char *fecha;
    int cantProductosComprados;
    struct Producto **productosComprados;   //vector de productos
    struct Sede *SedeCompra;                //referencia a Sede en la que se realizo la compra
};

struct NodoCompras{
    struct Compra *datosCompra;
    struct NodoCompras *sig, *ant;          //lista doblemente enlazada
};

struct Producto{
    char *marca;
    int id;
    char *modelo;                           //variable solo valida para autos
    char *nombre;                           //variable solo valida para accesorios y repuestos
    int precio;
    int cantRecalls;                        //variable solo valida para autos
};

struct NodoProductos{
    struct Producto *datosProducto;
    struct NodoProductos *izq, *der;        //arbol binario de busqueda
};

//FUNCIONES BASICAS REGIONES

/* Funcion crea un nuevo struct Region. El usuario introduce interactivamente los valores para crear la nueva region.
 * Se verifica que el ID introducido sea valido, es decir un valor positivo mayor a 0. En cualquier otro caso 
 * retorna el struct creado */
struct Region *crearRegion(){
    struct Region *nueva;
    char buffer[100];

    nueva = (struct Region*) malloc(sizeof(struct Region));

    printf("ID Region: ");

    while(1){
        scanf("%d", &(nueva->id));
        if(nueva->id > 0) break;
        printf("Ingrese ID valido\n");
    }

    nueva->cantSedes = 0;

    printf("Nombre: ");
    scanf(" %[^\n]s",buffer);

    nueva->nombre = (char*) malloc (strlen(buffer)*sizeof(char));
    strcpy(nueva->nombre, buffer);

    nueva->sedes = NULL;

    printf("\n");

    return nueva;
}

/* Funcion recibe la lista de regiones e imprime en pantalla los datos de cada una.
 * En caso de no existir regiones se imprime el mensaje correspondiente */
void listarRegiones(struct NodoRegion *headRegiones){
    struct NodoRegion *rec;

    if(!headRegiones){
        printf("No hay regiones\n\n");
        return;
    }

    rec = headRegiones;

    do{
        printf("Nombre: %s\n",rec->datosRegion->nombre);
        printf("ID: %d\n",rec->datosRegion->id);
        printf("Cantidad de sedes: %d\n\n",rec->datosRegion->cantSedes);
        rec = rec->sig;
    }while(rec != headRegiones);
}

/* Funcion recibe la lista regiones y el ID de la region buscada. Se recorre la lista y si se encuentra
 * la region buscada se retorna el struct de esta. En el caso que la region de ID buscada no exista se retorna NULL */
struct Region *buscarRegion(struct NodoRegion *headRegiones, int idBuscado){
    struct NodoRegion *rec;

    if(headRegiones){
        rec = headRegiones;

        do{
            if(rec->datosRegion->id == idBuscado) return rec->datosRegion;
            rec = rec->sig;
        }while(rec != headRegiones);
    }
    return NULL;
}


/* Funcion recibe doble puntero de la lista regiones a la que se le agregara un nuevo elemento, y los datos de la nueva
 * region a agregar. Se asigna memoria para el nuevo nodo y segun el caso correspondiente se agrega el nuevo elemento.
 * En caso de agregar el nodo correctamente se retorna 1, de caso contrario se retorna 0 */
int agregarRegion(struct NodoRegion **headRegiones, struct Region *nuevaRegion){
    struct NodoRegion *rec, *nuevo;

    nuevo = (struct NodoRegion*) malloc(sizeof(struct NodoRegion));
    nuevo->datosRegion = nuevaRegion;

    if((*headRegiones) == NULL){                                   //Caso la lista esta vacia
        *headRegiones = nuevo;
        (*headRegiones)->sig = (*headRegiones);
        return 1;
    }

    if(!buscarRegion(*headRegiones, nuevaRegion->id)){    //Si lista no esta vacia, se verifica que no exista otra region con esa ID
        rec = *headRegiones;
        do{
            rec = rec->sig;
        }while(rec->sig != *headRegiones);

        rec->sig = nuevo;
        rec->sig->sig = *headRegiones;
        return 1;
    }
    return 0;
}

/* Funcion recibe doble puntero de la lista regiones a la que se le eliminara un nuevo elemento, y el ID de la region a eliminar
 * Antes de proceder se verifica que la lista exista y que el elemento a eliminar exista, si esta verificacion falla se
 * retorna 0, indicando que no se pudo eliminar el elemento. En el caso de eliminarse correctamente se retorna 1*/
int eliminarRegion(struct NodoRegion **headRegiones, int idEliminar){
    struct NodoRegion *rec;

    if(*headRegiones){
        if(!buscarRegion(*headRegiones, idEliminar)){           //Se verifica que elemento exista
            return 0;
        }

        if((*headRegiones)->datosRegion->id == idEliminar && (*headRegiones)->sig == (*headRegiones)){  //Caso en el que elemento a eliminar es el head y unico elemento en la lista
            (*headRegiones) = NULL;
            return 1;
        }

        rec = *headRegiones;
        do{
            rec = rec->sig;
        }while(rec->sig->datosRegion->id != idEliminar);

        if(rec->sig == *headRegiones){             //Caso elemento a eliminar es el head, pero hay mas elementos en la lista
            *headRegiones = (*headRegiones)->sig;
        }
        rec->sig = rec->sig->sig;
        return 1;
    }
    return 0;
}

/* Funcion recibe lista de regiones y el struct de la region a modificar. Usuario introduce los nuevos datos.
 * Se puede modificar el ID y nombre de la region. Si se desea mantener un valor se introduce '0' en el caso de las
 * variables numericas y '.' en el caso de strings. Si se modifica el ID de la region se verifica que no exista 
 * otra region con el mismo ID. En caso de modificar correctamente se retorna 1, en caso contrario retorna 0. */
int modificarRegion (struct NodoRegion *headRegiones, struct Region *regionModificar){
    int idNuevo = 0;
    char buffer[100], *nombreNuevo;

    printf("Nuevo ID: ");
    while(1){
    	scanf("%d", &(idNuevo));
        if(idNuevo >= 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Nuevo nombre: ");
    scanf(" %[^\n]", buffer);
    nombreNuevo = (char*) malloc(sizeof(char)*strlen(buffer));
    strcpy(nombreNuevo,buffer);

    if(!buscarRegion(headRegiones, idNuevo)){       //Si no existe region con el nuevo ID se procede con los cambios
        if(idNuevo)
            regionModificar->id = idNuevo;

        if(strcmp(nombreNuevo, ".") != 0)
            regionModificar->nombre = nombreNuevo;

        return 1;
    }

    printf("Error, el nuevo id ya existe.\n");
    return 0;
}

//FUNCIONES BASICAS SEDES

/* Funcion crea un nuevo struct Sede. El usuario introduce interactivamente los valores para crear la nueva sede.
 * Se verifica que el ID introducido sea valido, es decir un valor positivo mayor a 0. En cualquier otro caso 
 * retorna el struct creado */
struct Sede *crearSede(){
    struct Sede *nuevo;
    char buffer[100];
    size_t size;

    nuevo = (struct Sede*)malloc(sizeof(struct Sede));

    printf("ID Sede: ");
    while(1){
        scanf("%d", &(nuevo->idSede));
        if(nuevo->idSede > 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Direccion: ");
    scanf(" %[^\n]s",buffer);

    size = strlen(buffer);

    nuevo->direccion = (char*) malloc(size*sizeof(char));
    strcpy(nuevo->direccion, buffer);

    printf("Supervisor: ");
    scanf(" %[^\n]s",buffer);

    size = strlen(buffer);

    nuevo->supervisor = (char*) malloc(size*sizeof(char));
    strcpy(nuevo->supervisor, buffer);

    nuevo->stockProductos = NULL;

    printf("\n");
    return nuevo;
}

/* Funcion recibe la lista de sedes e imprime en pantalla los datos de cada una.
 * En caso de no existir sedes se imprime el mensaje correspondiente */
void listarSedes(struct NodoSedes *headSedes){
    struct NodoSedes *rec;

    if(!headSedes){
        printf("No tiene sedes registradas\n\n");
        return;
    }

    rec = headSedes;

    while(rec){
        printf("ID Sede: %d\n",rec->datosSede->idSede);
        printf("Direccion: %s\n",rec->datosSede->direccion);
        printf("Supervisor: %s\n\n", rec->datosSede->supervisor);
        rec = rec->sig;
    }
}

/* Funcion recibe la lista de sedes de una region y el ID de la sede buscada. Se recorre la lista y si se encuentra
 * la region buscada se retorna el struct de esta. En el caso que la sede de ID buscada no exista se retorna NULL */
struct Sede *buscarSede(struct NodoSedes *headSedes, int idSedeBuscado){
    struct NodoSedes *rec;

    if(headSedes){
        rec = headSedes;

        while(rec){
            if(rec->datosSede->idSede == idSedeBuscado) return rec->datosSede;
            rec = rec->sig;
        }
    }
    return NULL;
}

/* Funcion recibe doble puntero de la lista de sedes de una region a la que se le agregara un nuevo elemento, y los datos de la
 * nueva sede a agregar. Se asigna memoria para el nuevo nodo y segun el caso correspondiente se agrega el nuevo elemento.
 * En caso de agregar el nodo correctamente se retorna 1, de caso contrario se retorna 0*/
int agregarSede(struct NodoSedes **headSedes, struct Sede *nuevaSede){
    struct NodoSedes *rec, *nuevo;

    nuevo = (struct NodoSedes*) malloc(sizeof(struct NodoSedes));
    nuevo->datosSede = nuevaSede;

    if(*headSedes == NULL){                             //Caso la lista esta vacia
        *headSedes = nuevo;
        (*headSedes)->ant = (*headSedes)->sig = NULL;
        return 1;
    }

    if(!buscarSede(*headSedes, nuevaSede->idSede)){    //Se verifica que no exista otro elemento con la misma ID en la misma lista
        rec = *headSedes;
        while(rec->sig){
            rec = rec->sig;
        }
        rec->sig = nuevo;
        rec->sig->sig = NULL;
        rec->sig->ant = rec;
        return 1;
    }
    return 0;
}

/* Funcion recibe doble puntero de la lista de sedes de una region a la que se le eliminara un nuevo elemento, y el ID de la sede
 * a eliminar. Antes de proceder se verifica que la lista exista y que el elemento a eliminar exista, si esta verificacion
 * falla se retorna 0, indicando que no se pudo eliminar el elemento. En el caso de eliminarse correctamente se retorna 1*/
int eliminarSede(struct NodoSedes **headSedes, int idSedeEliminar){
    struct NodoSedes *rec;

    if(*headSedes){
        if(!buscarSede(*headSedes, idSedeEliminar)){
            return 0;
        }

        if((*headSedes)->datosSede->idSede == idSedeEliminar){          //Caso elemento a eliminar es el head
            *headSedes = (*headSedes)->sig;
            if((*headSedes)) (*headSedes)->ant = NULL;
            return 1;
        }

        rec = *headSedes;
        while(rec){
            if(rec->datosSede->idSede == idSedeEliminar){
                break;
            }
            rec = rec->sig;
        }

        if(rec->sig){                  //Caso es un elemento en el medio de la lista
            rec->ant->sig = rec->sig;
            rec->sig->ant = rec->ant;
            rec = NULL;
        }

        else{                       //Caso es el ultimo elemento en la lista
            rec->ant->sig = NULL;
        }

        return 1;
    }
    return 0;
}

/* Funcion recibe lista de regiones y el struct de la sede a modificar. Usuario introduce los nuevos datos.
 * Se puede modificar el ID, direccion y supervisor de una sede. Si se desea mantener un valor se introduce '0' en el caso de
 * las variables numericas y '.' en el caso de strings. Si se modifica el ID de la sede se verifica que no exista otra sede 
 * con el mismo ID en cada una de las regiones. En caso de modificar correctamente se retorna 1, en caso contrario retorna 0. */
int modificarSede(struct NodoRegion* headRegiones, struct Sede *sedeModificar){
    struct NodoRegion *recRegion;
    int idNueva = 0;
    char *direccionNueva = NULL, *supervisorNuevo = NULL, buffer[100];

    printf("Nueva ID: ");
    while(1){
        scanf("%d", &(idNueva));
        if(idNueva >= 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Nueva direccion: ");
    scanf(" %[^\n]", buffer);

    direccionNueva = (char*)malloc(sizeof(char)*strlen(buffer));
    strcpy(direccionNueva, buffer);

    printf("Nuevo supervisor: ");
    scanf(" %[^\n]", buffer);

    supervisorNuevo = (char*)malloc(sizeof(char)*strlen(buffer));
    strcpy(supervisorNuevo, buffer);

    printf("\n");

    recRegion = headRegiones;

    if(idNueva){    //Si se desea modificar la ID se verifica que no exista otra sede con esa ID
        do{
            if(buscarSede(recRegion->datosRegion->sedes, idNueva)){
                printf("Ya existe sede con esa ID\n");
                return 0;
            }
            recRegion = recRegion->sig;
        }while(recRegion != headRegiones);
    }

    if(idNueva)
        sedeModificar->idSede = idNueva;

    if(strcmp(direccionNueva,".") != 0)
        sedeModificar->direccion = direccionNueva;

    if(strcmp(supervisorNuevo,".") != 0)
        sedeModificar->supervisor = supervisorNuevo;

    return 1;
}

//FUNCIONES BASICAS PRODUCTOS

/* Funcion crea un nuevo struct Producto. El usuario introduce interactivamente los valores para crear el nuevo producto.
 * Se verifica que el ID introducido sea valido, es decir un valor positivo mayor a 0. En cualquier otro caso retorna el struct creado
 * Si usuario introduce un modelo se considera que el producto es un Auto y se asigna un valor en la cantidad de recalls
 * Si no se introduce un modelo (usuario introduce '.') se le asigna un nombre al producto debido a que es un repuesto o accesorio */
struct Producto *crearProducto(){
    struct Producto *nuevo;
    char buffer[100];

    nuevo = (struct Producto*)malloc(sizeof(struct Producto));

    printf("ID Producto: ");
    while(1){
        scanf("%d", &(nuevo->id));
        if(nuevo->id > 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Marca: ");
    scanf(" %[^\n]s",buffer);

    printf("Precio: ");
    scanf("%d",&(nuevo->precio));

    nuevo->marca = (char*) malloc(strlen(buffer)*sizeof(char));
    strcpy(nuevo->marca, buffer);

    printf("Modelo: ");
    scanf(" %[^\n]s",buffer);

    if(strcmp(buffer,".") == 0){      //Si no se agrega un modelo, se procede a considerar como un accesorio/repuesto
        nuevo->modelo = NULL;

        printf("Nombre: ");
        scanf(" %[^\n]s",buffer);

        nuevo->nombre = (char*) malloc(strlen(buffer)*sizeof(char));
        strcpy(nuevo->nombre, buffer);
    }

    else{   //En el caso que se agregue modelo, se considera producto como un auto y se agrega cantidad de recalls
        nuevo->nombre = NULL;

        nuevo->modelo = (char*) malloc(strlen(buffer)*sizeof(char));
        strcpy(nuevo->modelo, buffer);

        printf("Cantidad Recalls: ");
        scanf("%d",&(nuevo->cantRecalls));
    }

    printf("\n");

    return nuevo;
}

/* Funcion recursiva para recorrer un Arbol Binario de Busqueda (ABB) de forma pre-orden. Si existe Nodo se imprime
 * en pantalla los datos del producto. En el caso que no exista modelo se imprime nombre debido a que producto es
 * un accesorio/producto. Si existe un modelo se sabe que producto es un auto por lo que se imprime cantidad de recalls */
void listarProductos(struct NodoProductos *productos){
    if(productos != NULL){

        printf("ID producto: %d\n", productos->datosProducto->id);
        printf("Marca producto: %s\n", productos->datosProducto->marca);

        if(!(productos->datosProducto->modelo)) {
            printf("Nombre producto: %s\n", productos->datosProducto->nombre);
        }

        else{
            printf("Modelo: %s\n", productos->datosProducto->modelo);
            printf("Cantidad de recalls: %d\n", productos->datosProducto->cantRecalls);
        }

        printf("Precio: %d\n\n", productos->datosProducto->precio);

        listarProductos(productos->izq);
        listarProductos(productos->der);
    }
}

/* Funcion recursiva para recorrer ABB de productos en pre-orden. Recibe el ABB de productos y el ID del producto buscado
 * Si existe nodo se verifica que el ID de ese producto coincida con el ID buscado. Proceso se repite hasta encontrar el
 * producto o recorrer todos los nodos. En el caso de encontrar producto retorna su struct de datos, en el caso contrario
 * retorna NULL */
struct Producto *buscarProducto(struct NodoProductos *abbPro, int idBuscado){
    if(!abbPro || abbPro->datosProducto->id == idBuscado){
        if(!abbPro) return NULL;
        return abbPro->datosProducto;
    }

    else{   //Si el ID Buscado es menor recorremos hacia la izquierda, en caso contrario hacia la derecha
        if (abbPro->datosProducto->id > idBuscado)
            return buscarProducto(abbPro->izq, idBuscado);
        else
            return buscarProducto(abbPro->der, idBuscado);
    }
}

/* Funcion recursiva que agrega un Nodo a un ABB de forma recursiva en pre-orden. Recibe el ABB de productos y los datos del
 * nuevo producto a agregar. Al llegar a la posicion correcta para agregar el nuevo Nodo se reserva memoria y se guardan los datos*/
void agregarProducto(struct NodoProductos **abbPro, struct Producto *producto){
    if((*abbPro)){              //Caso existe un producto con la misma ID
        if((*abbPro)->datosProducto->id == producto->id){
            printf("Ya existe producto con esa ID\n\n");
            return;
        }
    }

    if(!(*abbPro)){         //Se llega a la "posicion" vacia para agregar
        printf("Producto agregado exitosamente\n\n");
        (*abbPro) = (struct NodoProductos *)malloc(sizeof(struct NodoProductos));
        (*abbPro)->datosProducto = producto;
        (*abbPro)->izq = (*abbPro)->der = NULL;
    }

    //Si aun no llegamos a la posicion en la que agregar, recorremos hacia la izquierda en caso que el ID a agregar sea menor, en caso contrario recorremos hacia la derecha
    else if((*abbPro)->datosProducto->id > producto->id)
        agregarProducto(&((*abbPro)->izq), producto);
    else
        agregarProducto(&((*abbPro)->der), producto);
}

/* Funcion recursiva necesaria para implementar la funcion de eliminar producto. Recibe doble puntero al ABB y doble puntero del nodo
 * del producto que se reemplazara. La funcion se llama en el caso que se elimine un nodo con mas de un hijo */
void reemplazarProducto(struct NodoProductos **abb, struct NodoProductos **remp){
    if((*abb)->der == NULL){
        (*remp)->datosProducto = (*abb)->datosProducto;
        *remp = *abb;
        *abb = (*abb)->izq;
    }
    else
        reemplazarProducto(&(*abb)->der, &(*remp));
}

/* Funcion recursiva que recibe puntero al ABB productos y el ID del producto a eliminar. Se recorre ABB buscando el nodo
 * a eliminar. Al encontrarlo se elimina considerando los casos en el que tenga un solo hijo, o mas */
void eliminarProducto(struct NodoProductos **nPro, int idBuscado){
    struct NodoProductos *remp;
    if(*nPro == NULL) return;

    //Se busca el nodo a eliminar
    if((*nPro)->datosProducto->id < idBuscado)
        return eliminarProducto(&(*nPro)->der, idBuscado);
    else if((*nPro)->datosProducto->id > idBuscado)
        return eliminarProducto(&(*nPro)->izq, idBuscado);

    else{   //Encuentra el nodo a eliminar
        //Casos en los que el nodo tenga hijos
        remp = *nPro;

        //Tiene hijo solo en la derecha
        if((*nPro)->izq == NULL){
            *nPro = (*nPro)->der;
        }

        //Tiene hijo solo en la izquierda
        else if((*nPro)->der == NULL){
            *nPro = (*nPro)->izq;
        }

        //Caso de tener mas hijos
        else
            reemplazarProducto(&(*nPro)->izq, &remp);
        free(remp);
    }
}

/* Funcion recibe ABB productos y el struct del producto a modificar. Usuario ingresa los nuevos datos para el struct.
 * Se pueden modificar la marca, nombre/modelo (depende del tipo de producto) y precio. Si se desea mantener un valor
 * se introduce '0' para las variables numericas y '.' para las strings. */
void modificarProducto(struct NodoProductos *abbProductos, struct Producto *productoModificar){
    char *nuevaMarca = NULL, *nuevoModelo = NULL, *nuevoNombre = NULL;
    char buffer[100];
    int precioNuevo;

    printf("Ingrese nueva marca: ");
    scanf(" %[^\n]s", buffer);
    nuevaMarca = (char*)malloc(sizeof(char)*(strlen(buffer)));
    strcpy(nuevaMarca, buffer);

    if(productoModificar->nombre != NULL){  //Caso producto no es un auto
        printf("Ingrese nuevo nombre: ");
        scanf(" %[^\n]s", buffer);
        nuevoNombre = (char*)malloc(sizeof(char)*(strlen(buffer)));
        strcpy(nuevoNombre, buffer);
    }

    else{
        printf("Ingrese nuevo modelo: ");
        scanf(" %[^\n]s", buffer);
        nuevoModelo = (char*)malloc(sizeof(char)*(strlen(buffer)));
        strcpy(nuevoModelo, buffer);
    }

    printf("Ingrese nuevo precio: ");
    scanf("%d", &precioNuevo);

    if(strcmp(nuevaMarca, ".") != 0)
        productoModificar->marca = nuevaMarca;

    if(nuevoModelo){
        if(strcmp(nuevoModelo, ".") != 0)
            productoModificar->modelo = nuevoModelo;
    }

    else{
        if(strcmp(nuevoNombre, ".") != 0)
            productoModificar->nombre = nuevoNombre;
    }

    if(precioNuevo != 0)
        productoModificar->precio = precioNuevo;
}

//FUNCIONES BASICAS CLIENTES

/* Funcion crea un nuevo struct Cliente. El usuario introduce interactivamente los valores para crear al nuevo cliente.
 * Se verifica que el ID introducido sea valido, es decir un valor positivo mayor a 0. En caso que el usuario introduzca
 * un valor no numerico en el ID, se retorna NULL y se cierra el programa. En cualquier otro caso retorna el struct creado */
struct Cliente *crearCliente(){
    struct Cliente *nuevo;
    char buffer[100];

    nuevo = (struct Cliente*) malloc(sizeof(struct Cliente));

    printf("ID Cliente: ");
    while(1){
        scanf("%d", &(nuevo->idCliente));
        if(nuevo->idCliente > 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Nombre: ");
    scanf(" %[^\n]s",buffer);

    nuevo->nombre = (char*) malloc (strlen(buffer)*sizeof(char));
    strcpy(nuevo->nombre, buffer);

    printf("RUT: ");
    scanf(" %[^\n]s",buffer);

    nuevo->rut = (char*) malloc (strlen(buffer)*sizeof(char));
    strcpy(nuevo->rut, buffer);

    nuevo->listaCompras = NULL;
    printf("\n");
    return nuevo;
}


/* Funcion recibe lista de clientes e imprime en pantalla los datos de cada uno. En caso de no exisitr ninguno se imprime
 * el mensaje correspondiente */
void listarClientes(struct NodoClientes *headClientes){
    struct NodoClientes *rec;

    if(!headClientes){
        printf("No hay clientes\n\n");
        return;
    }

    rec = headClientes;

    do{
        printf("Nombre cliente: %s\n",rec->datosCliente->nombre);
        printf("Rut: %s\n",rec->datosCliente->rut);
        printf("ID cliente: %d\n\n",rec->datosCliente->idCliente);
        rec = rec->sig;
    }while(rec != headClientes);
}

/* Funcion recibe la lista clientes y el ID del cliente buscado. Se recorre la lista y si se encuentra cliente buscado se
 * retorna el struct de este. En el caso que cliente no exista se retorna NULL */
struct Cliente *buscarCliente(struct NodoClientes *headClientes, int idClienteBuscado){
    struct NodoClientes *rec;

    if(headClientes){
        rec = headClientes;

        do{
            if(rec->datosCliente->idCliente == idClienteBuscado){
                return rec->datosCliente;
            }
            rec = rec->sig;
        }while(rec != headClientes);
    }
    return NULL;
}

/* Funcion recibe doble puntero de la lista clientes a la que se le agregara un nuevo elemento, y los datos del nuevo cliente
 * a agregar. Se asigna memoria para el nuevo nodo y segun el caso correspondiente se agrega el nuevo elemento. En caso de
 * agregar el nodo correctamente retorna 1, de caso contrario retorna 0 */
int agregarCliente(struct NodoClientes **headClientes, struct Cliente *nuevoCliente){
    struct NodoClientes *rec, *nuevo;

    nuevo = (struct NodoClientes*) malloc(sizeof(struct NodoClientes));
    nuevo->datosCliente = nuevoCliente;

    if(*headClientes == NULL){      //Caso la lista esta vacia
        *headClientes = nuevo;
        (*headClientes)->sig = (*headClientes);
        return 1;
    }

    if(buscarCliente(*headClientes, nuevoCliente->idCliente) == 0){     //Si lista no esta vacia se verifica que no exista otro cliente con el mismo ID
        rec = *headClientes;
        do{
            rec = rec->sig;
        }while(rec->sig != *headClientes);

        rec->sig = nuevo;
        rec->sig->sig = *headClientes;
        return 1;
    }
    return 0;
}

/* Funcion recibe doble puntero de la lista clientes y el ID del cliente a eliminar. Antes se verifica que la lista exista
 * y que el cliente a eliminar se encuentre en la lista. Si falla esta verificacion se retorna 0. Si se elimina elemento
 * correctamente se retorna 1 */
int eliminarCliente(struct NodoClientes **headClientes, int idEliminar){
    struct NodoClientes *rec;

    if(*headClientes){
        if(buscarCliente(*headClientes, idEliminar)){
            if((*headClientes)->datosCliente->idCliente == idEliminar && (*headClientes)->sig == (*headClientes)){
                (*headClientes) = NULL;
                return 1;
            }

            rec = *headClientes;
            do{
                rec = rec->sig;
            }while(rec->sig->datosCliente->idCliente != idEliminar);

            if(rec->sig == *headClientes){
                *headClientes = (*headClientes)->sig;
            }
            rec->sig = rec->sig->sig;
            return 1;
        }
    }
    return 0;
}


/* Funcion recibe lista de clientes y el struct del cliente a modificar. Usuario introduce los nuevos datos. Se puede
 * modificar el ID, nombre y RUT del cliente. Si se desea mantener un valor se introduce '0' para las variables numericas
 * o '.' para las strings. Si se modifica la ID se verifica que no exista otro cliente con el mismo ID. Se retorna 0 o 1 
 * segun exito de la modificacion*/
int modificarCliente(struct NodoClientes *headClientes, struct Cliente *clienteModificar){
    int idNuevo = 0;
    char buffer[100], *nombreNuevo, *rutNuevo;

    printf("Nuevo ID: ");
    while(1){
        scanf("%d", &(idNuevo));
        if(idNuevo >= 0) break;
        printf("Ingrese ID valido\n");
    }

    printf("Nuevo nombre: ");
    scanf(" %[^\n]", buffer);
    nombreNuevo = (char*) malloc(sizeof(char) *strlen(buffer));
    strcpy(nombreNuevo,buffer);

    printf("Nuevo RUT: ");
    scanf(" %[^\n]", buffer);
    rutNuevo = (char*) malloc(sizeof(char) *strlen(buffer));
    strcpy(rutNuevo,buffer);


    if(!buscarCliente(headClientes, idNuevo)){ //Se busca si existe algun cliente con el ID nuevo
        if(idNuevo)
            clienteModificar->idCliente = idNuevo;
        if(strcmp(rutNuevo,".") != 0)
            clienteModificar->rut = rutNuevo;
        if(strcmp(nombreNuevo,".") != 0)
            clienteModificar->nombre = nombreNuevo;
        return 1;
    }

    else{
        printf("Error, el nuevo id ya existe.\n");
        return 0;
    }
}

//FUNCIONES BASICAS COMPRAS

/* Funcion crea un nuevo struct Compra. Recibe como argumento el struct Sede en la que se realizo la compra.
 * El usuario introduce interactivamente los datos para crear nueva compra. Se verifica que valor de ID y cantidad de
 * compras sean validos. Se agregan los productos buscando su ID en la sede que sea realiza la compra. Se retorna NULL
 * en caso de error, sino retorna el struct creado. */
struct Compra *crearCompra(struct Sede *sedeDeCompra){
    struct Compra *nuevo;
    struct Producto *producto;
    char buffer[100];
    int i = 0, idProducto;

    if(!sedeDeCompra->stockProductos) return NULL;  //Si la sede de compra no tiene productos se retorna NULL

    nuevo = (struct Compra*)malloc(sizeof(struct Compra));

    printf("ID boleta: ");
    while(1){                                     
        scanf("%d", &(nuevo->idBoleta));      
        if(nuevo->idBoleta > 0) break;
        printf("Ingrese ID valido\n");
    }

    nuevo->montoTotal = 0;

    printf("Cantidad de productos comprados: ");

    while(1){
        scanf("%d", &(nuevo->cantProductosComprados));
        if(nuevo->cantProductosComprados > 0){
            break;
        }
        else{
            printf("Ingrese cantidad de productos valida\n");
        }
    }

    printf("Fecha: ");
    scanf(" %[^\n]s",buffer);

    nuevo->fecha = (char*) malloc(strlen(buffer)*sizeof(char));
    strcpy(nuevo->fecha, buffer);

    nuevo->SedeCompra = sedeDeCompra;
    printf("\n");

    nuevo->productosComprados = (struct Producto**) malloc(sizeof(struct Producto*)*nuevo->cantProductosComprados);

    //Se pobla vector con los productos comprados
    while(1){
        if (i >= nuevo->cantProductosComprados) return nuevo;
        printf("Ingrese IDs de cada producto: ");
        scanf("%d", &idProducto);

        producto = buscarProducto(sedeDeCompra->stockProductos, idProducto);

        if(!producto)
            printf("ID no encontrado. Ingrese ID valido\n");
        else{
            printf("Producto agregado\n");
            nuevo->productosComprados[i] = producto;
            nuevo->montoTotal += producto->precio;
            i++;
        }
        printf("\n");
    }
    return nuevo;
}

// Funcion recibe lista de compras de un cliente y muestra en pantalla los datos de cada una.
void listarCompras(struct NodoCompras *headCompras){
    struct NodoCompras *rec;
    int i;

    if(!headCompras){
        printf("No tiene compras registradas\n\n");
        return;
    }

    rec = headCompras;

    while(rec){
        printf("ID compra: %d\n",rec->datosCompra->idBoleta);
        printf("Monto total compra: %d\n", rec->datosCompra->montoTotal);
        printf("Fecha: %s\n", rec->datosCompra->fecha);
        printf("Compra realizada en sede ID: %d\n\n", rec->datosCompra->SedeCompra->idSede);
        printf("Productos comprados:\n");

        for(i = 0 ; i<rec->datosCompra->cantProductosComprados ; i++){
            printf("Marca: %s | ", rec->datosCompra->productosComprados[i]->marca);

            if(rec->datosCompra->productosComprados[i]->nombre != NULL){
                printf("Nombre: %s\n", rec->datosCompra->productosComprados[i]->nombre);
            }
            else{
                printf("Modelo: %s\n", rec->datosCompra->productosComprados[i]->modelo);
            }
        }
        printf("\n");
        rec = rec->sig;
    }
}

/* Funcion recibe lista de compras de un cliente y el ID de la compra buscada. Se recorre lista hasta encontrar la compra
 * retornando su struct. En caso que no exista compra con ese ID se retorna NULL */
struct Compra *buscarCompra(struct NodoCompras *headCompras, int idBoletaBuscado){
    struct NodoCompras *rec;

    if(headCompras){
        rec = headCompras;

        while(rec){
            if(rec->datosCompra->idBoleta == idBoletaBuscado) return rec->datosCompra;
            rec = rec->sig;
        }
    }
    return NULL;
}

/* Funcion recibe doble puntero de la lista compra a la que se le agregara un nuevo elemento, y los datos de la nueva compra.
 * En caso de agregar correctamente se retorna 1, de caso contrario se reotnra 0 */
int agregarCompra(struct NodoCompras **headCompras, struct Compra *nuevaCompra){
    struct NodoCompras *rec, *nuevo;

    nuevo = (struct NodoCompras*) malloc(sizeof(struct NodoCompras));
    nuevo->datosCompra = nuevaCompra;

    if(*headCompras == NULL){          //Caso la lista esta vacia
        *headCompras = nuevo;
        (*headCompras)->ant = (*headCompras)->sig = NULL;
        return 1;
    }

    if(buscarCompra(*headCompras, nuevaCompra->idBoleta) == 0){ //Se verifica que no exista otra compra con la misma ID
        rec = *headCompras;
        while(rec->sig){
            rec = rec->sig;
        }
        rec->sig = nuevo;
        rec->sig->sig = NULL;
        rec->sig->ant = rec;
        return 1;
    }
    return 0;
}

/* Funcion recibe doble puntero de la lista compra a la que se le eliminara un elemento y el ID de la compra a eliminar.
 * En caso de eliminar correctamente se retorna 1, en caso contrario 0 */
int eliminarCompra(struct NodoCompras **headCompras, int idBoletaEliminar){
    struct NodoCompras *rec;

    if(*headCompras){
        if(!buscarCompra(*headCompras, idBoletaEliminar)){
            return 0;
        }

        if((*headCompras)->datosCompra->idBoleta == idBoletaEliminar){
            *headCompras = (*headCompras)->sig;
            if((*headCompras)) (*headCompras)->ant = NULL;
            return 1;
        }

        rec = *headCompras;

        while(rec){
            if(rec->datosCompra->idBoleta == idBoletaEliminar){
                break;
            }
            rec = rec->sig;
        }

        if(rec->sig){
            rec->ant->sig = rec->sig;
            rec->sig->ant = rec->ant;
            rec = NULL;
        }

        else{
            rec->ant->sig = NULL;
        }

        return 1;
    }
    return 0;
}

/* Funcion recibe lista de compras y el struct de la compra a modificar. Usuario introduce los nuevos datos. Se puede
 * modificar el ID y fecha de la compra. Si modifica correctamente retorna 1, en caso contrario 0 */
int modificarCompra(struct NodoClientes *headClientes, struct Compra *compraModificar){
    struct NodoClientes *recClientes;
    int idNueva = 0;
    char *fechaNueva = NULL, buffer[100];


    printf("Nueva ID: ");
    scanf("%d", &idNueva);

    printf("Nueva fecha: ");
    scanf(" %[^\n]", buffer);

    fechaNueva = (char*)malloc(sizeof(char)*strlen(buffer));
    strcpy(fechaNueva, buffer);

    printf("\n");

    recClientes = headClientes;

    if(idNueva){
        do{
            if(buscarCompra(recClientes->datosCliente->listaCompras, idNueva)){
                printf("Ya existe sede con esa ID\n");
                return 0;
            }
            recClientes = recClientes->sig;
        }while(recClientes != headClientes);
    }

    if(idNueva)
        compraModificar->idBoleta = idNueva;

    if(strcmp(fechaNueva,".") != 0)
        compraModificar->fecha = fechaNueva;

    return 1;
}

//FUNCIONES ADICIONALES

// Funcion imprime las marcas y modelos solamente de los productos que sean autos.
//Se incluye un "bool" para saber si hay al menos un auto en la sede, para luego imprimir los mensajes correspondientes
void mostrarMarcasDeAuto(struct NodoProductos *abb, int *bool){
    if (abb)
    {
        if(abb->datosProducto->modelo){ // Si el modelo es NULL no es auto
            *bool = 1;
            printf("Marca : %s - Modelo: %s\n",abb->datosProducto->marca,abb->datosProducto->modelo);
        }
        mostrarMarcasDeAuto(abb->izq, bool);
        mostrarMarcasDeAuto(abb->der, bool);
    }
}

/* Funcion recorre todas las regiones y sus sedes registradas, llamando a funcion mostrarMarcasDeAuto para imprimir todas
 * las marcas y modelos de autos. Se consideran los casos especiales, como por ejemplo: No existan sedes en la region,
 * sede no tenga productos, o sede no tiene autos */
void modelosMarcasVendidosPorZona(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoSedes *recSede;
    int hayAutos = 0;

    recRegion = DercoChile->regiones;

    printf("Marcas y modelos de autos por cada region:\n");
    do {
        if(recRegion){
            printf("Region: %s | ID: %d\n",recRegion->datosRegion->nombre,recRegion->datosRegion->id);
            recSede = recRegion->datosRegion->sedes;

            if(!recSede){
                printf("No hay sedes en la region!\n\n");
            }

            while(recSede != NULL){
                printf("Sede de ID %d:\n",recSede->datosSede->idSede);

                if(!recSede->datosSede->stockProductos){
                    printf("Sede no tiene productos\n\n");
                }

                else{
                    mostrarMarcasDeAuto(recSede->datosSede->stockProductos, &hayAutos);
                    if(hayAutos == 0) printf("No hay autos en la sede\n");
                    printf("\n");
                    hayAutos = 0;
                }
                recSede = recSede->sig;
            }
        }
        recRegion = recRegion->sig;
    }while(recRegion != DercoChile->regiones);
}

//**********************************************************************************************************************

/* Funcion compara el struct del auto guardado que tenga mas recalls hasta el momento, si el auto con el que estamos
 * comparando tiene mayor cantidad de recalls reemplazamos el struct que tuviesemos guardado */
void compararCantRecallsAuto(struct NodoProductos *abb, int *mayor, struct Producto **autoConMasRecalls){
    if(abb){
        if(abb->datosProducto->modelo != NULL){                     // Solo los autos tienen modelo
            if((*mayor) < abb->datosProducto->cantRecalls){
                (*mayor) = abb->datosProducto->cantRecalls;
                (*autoConMasRecalls) = abb->datosProducto;
            }
        }
        compararCantRecallsAuto(abb->izq, mayor, autoConMasRecalls);
        compararCantRecallsAuto(abb->der, mayor, autoConMasRecalls);
    }
}

/* Funcion recorre listas de regiones y la lista de sedes de cada una y llama a la funcion de comparar cantidad de recalls.
 * Tras revisar todas las sedes del sistema y sus productos se retorna el struct Producto del auto con mas recalls. En el
 * caso que no existan vehiculos con recalls se retornara NULL */
struct Producto *autoConMasRecalls(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegiones;
    struct NodoSedes *recSedes;
    struct Producto *masRecalls = NULL;
    int cantRecalls = 0;

    if(!DercoChile->regiones) return NULL;

    recRegiones = DercoChile->regiones;

    do{
        recSedes = recRegiones->datosRegion->sedes;
        while(recSedes != NULL){
            compararCantRecallsAuto(recSedes->datosSede->stockProductos, &cantRecalls, &masRecalls);
            recSedes = recSedes->sig;
        }
        recRegiones = recRegiones->sig;
    }while(recRegiones!=DercoChile->regiones);

    return masRecalls;
}

//**********************************************************************************************************************

// Funcion recibe struct cliente y calcula el monto total de dinero que ha gastado
int contarDineroGastado(struct Cliente *cliente){
    struct NodoCompras *recCompra;
    int cont = 0;

    if(!cliente->listaCompras) return 0;

    recCompra = cliente->listaCompras;

    while(recCompra!=NULL){
        cont += recCompra->datosCompra->montoTotal;
        recCompra = recCompra->sig;
    }

    return cont;
}

/* Se recorre lista de clientes y se retorna el cliente que mas dinero haya gastado. En caso de no existir clientes se
 * retorna NULL. En el caso que ningun cliente tenga compras realizadas se retornara el primero */
struct Cliente *clienteQueMasGasta(struct SistemaDerco *DercoChile){
    struct NodoClientes *recCliente;
    struct Cliente *mayor = NULL;
    int montoMayor = 0;

    if(!DercoChile->clientes) return NULL;

    recCliente = DercoChile->clientes;

    mayor = recCliente->datosCliente;
    montoMayor = contarDineroGastado(recCliente->datosCliente);

    do{
        if(contarDineroGastado(recCliente->datosCliente) > montoMayor){
            mayor = recCliente->datosCliente;
            montoMayor = contarDineroGastado(recCliente->datosCliente);
        }
        recCliente = recCliente->sig;
    }while(recCliente != DercoChile->clientes);

    return mayor;
}

//**********************************************************************************************************************

/* Funcion recibe lista de compras de un cliente e ID de la sede buscada. Se recorre lista compras y se cuenta la cantidad
 * de productos vendidos en una sede en especifico.*/
int contadorCantProductos(struct NodoCompras *listaCompras, int idSedeBuscada){
    struct NodoCompras *recCompras;
    int contadorProductos = 0;

    if(listaCompras){
        recCompras = listaCompras;
        while(recCompras){
            if(recCompras->datosCompra->SedeCompra->idSede == idSedeBuscada)
                contadorProductos += recCompras->datosCompra->cantProductosComprados;

            recCompras = recCompras->sig;
        }
    }
    return contadorProductos;
}

//Recorre lista clientes para acceder a sus compras, contando la cantidad de productos vendido en una sede especifica
int contadorCantVentasSede(int idSedeBuscada, struct NodoClientes *headClientes){
    struct NodoClientes *rec;
    int contador = 0;

    rec = headClientes;
    do{
        contador += contadorCantProductos(rec->datosCliente->listaCompras, idSedeBuscada);
        rec = rec->sig;
    }while(rec != headClientes);

    return contador;
}

/* Funcion recorre lista regiones y las sedes de cada una llamando la funcion de contarCantVentasSede para encontrar la
 * sede que mayor cantidad de productos haya vendido. Se retorna la sede que mas productos haya vendido. En caso de empate
 * se retorna la primera */
struct Sede *sedeMayorDistribucion(struct SistemaDerco *DercoChile){
    struct NodoSedes *recSedes;
    struct NodoRegion *recRegiones;
    struct Sede *sedeMayor = NULL;
    int contador, maxCompras = 0;

    if(!DercoChile->regiones || !DercoChile->clientes) return NULL;

    recRegiones = DercoChile->regiones;
    do{
        recSedes = recRegiones->datosRegion->sedes;
        while(recSedes != NULL){
            contador = contadorCantVentasSede(recSedes->datosSede->idSede, DercoChile->clientes);

            if(sedeMayor == NULL && contador != 0){
                sedeMayor = recSedes->datosSede;
                maxCompras = contador;
            }

            else{
                if(contador > maxCompras){
                    sedeMayor = recSedes->datosSede;
                    maxCompras = contador;
                }
            }
            recSedes = recSedes->sig;
        }
        recRegiones = recRegiones->sig;
    }while(recRegiones != DercoChile->regiones);

    return sedeMayor;
}

//**********************************************************************************************************************

// Funcion recibe lista de clientes y suma el monto de todas sus compras.
int contadorComprasClientes(struct NodoClientes *headClientes){
    struct NodoClientes *recClientes;
    int contador = 0;

    if(headClientes == NULL) return 0;

    recClientes = headClientes;
    do{
        contador += contarDineroGastado(recClientes->datosCliente);
        recClientes = recClientes->sig;
    }while(recClientes != headClientes);

    return contador;
}

//Se imprime en pantalla el monto total recaudado, en caso de ser 0 se imprime que no hay ventas registradas.
void dineroRecaudado(struct SistemaDerco *DercoChile){
    int contadorDineroRecaudado;

    contadorDineroRecaudado = contadorComprasClientes(DercoChile->clientes);

    if(contadorDineroRecaudado != 0){
        printf("El dinero total recaudado es: $%d\n\n", contadorDineroRecaudado);
    }
    else{
        printf("Error. No hay compras registradas\n\n");
    }
}

//*****************************************************************************************

/* Funcion recibe sistema Derco. Se calcula el monto total recaudado por la empresa utilizando la funcion contadorComprasClientes
 * y calculamos la cantidad de sedes existentes para calcular el promedio de ventas por sede. Se retorna el promedio calculado */
float calcularPromedioVentas(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoSedes *recSede;
    int montoTotal, cantSedes = 0;

    if(!DercoChile->clientes) return 0;
    if(!DercoChile->regiones) return 0;

    montoTotal = contadorComprasClientes(DercoChile->clientes);

    if(montoTotal == 0) return 0;
    
    recRegion = DercoChile->regiones;

    do{
        recSede = recRegion->datosRegion->sedes;

        while(recSede){
            cantSedes += 1;
            recSede = recSede->sig;
        }

        recRegion = recRegion->sig;
    }while(recRegion != DercoChile->regiones);

    if(cantSedes == 0) return 0;

    return montoTotal/cantSedes;
}

// Se recorre la lista clientes y sus compras para calcular el monto vendido por cada sede
int contarMontoPorSede(int idSede, struct NodoClientes *listaClientes){
    struct NodoClientes *recCliente;
    struct NodoCompras *recCompra;
    int cont = 0;

    recCliente = listaClientes;

    do{
        recCompra = recCliente->datosCliente->listaCompras;
        while(recCompra){
            if(recCompra->datosCompra->SedeCompra->idSede == idSede)
                cont += recCompra->datosCompra->montoTotal;
            recCompra = recCompra->sig;
        }

        recCliente = recCliente->sig;
    }while(recCliente != listaClientes);

    return cont;
}

// Funcion calcula cantidad de sedes con ventas bajo el promedio
int cantidadSedesBajoPromedio(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoSedes *recSede;
    int cantSedesBajoPromedio = 0;
    float promedioVentas;

    promedioVentas = calcularPromedioVentas(DercoChile);
    if(promedioVentas == 0) return 0;

    recRegion = DercoChile->regiones;
    do{
        recSede = recRegion->datosRegion->sedes;
        while(recSede){
            if(contarMontoPorSede(recSede->datosSede->idSede, DercoChile->clientes) < promedioVentas){
                cantSedesBajoPromedio += 1;
            }
            recSede = recSede->sig;
        }
        recRegion = recRegion->sig;
    }while(recRegion != DercoChile->regiones);

    return cantSedesBajoPromedio;
}

//Funcion almacena en un vector todas las sedes con ventas bajo el promedio y lo retorna.
struct Sede **sedesBajoPromedioVentas(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoSedes *recSede;
    struct Sede **arr;
    int cantSedesBajoPromedio, i = 0;
    float promedioVentas;

    promedioVentas = calcularPromedioVentas(DercoChile);
    if(promedioVentas == 0) return NULL;
    if(!DercoChile->regiones) return NULL;
	
    cantSedesBajoPromedio = cantidadSedesBajoPromedio(DercoChile);
    if(cantSedesBajoPromedio == 0) return NULL;

    arr = (struct Sede**) malloc(sizeof(struct Sede*) * cantSedesBajoPromedio);

    recRegion = DercoChile->regiones;
    do{
        recSede = recRegion->datosRegion->sedes;
        while(recSede){
            if(contarMontoPorSede(recSede->datosSede->idSede, DercoChile->clientes) < promedioVentas){
                arr[i] = recSede->datosSede;
                i++;
            }
            recSede = recSede->sig;
        }
        recRegion = recRegion->sig;
    }while(recRegion != DercoChile->regiones);

    return arr;
}

//*********************************************

//Funcion imprime en pantalla la informacion de la empresa
void mostrarDatosEmpresa(struct SistemaDerco *DercoChile){
    if(!DercoChile) return;

    printf("CEO: %s\n", DercoChile->ceo);
    printf("RUT empresa: %s\n", DercoChile->rutEmpresarial);
    printf("Cantidad empleados en todo Chile: %d\n\n", DercoChile->cantEmpleados);
}

//MENUS

//Menu de las funciones de listar de los distintos structs
int menuListar(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoClientes *recCliente;
    struct NodoSedes *recSedes;

    int opcion;

    printf("Elija elemento a Listar:\n\n");
    printf("1. Listar regiones\n");
    printf("2. Listar sedes\n");
    printf("3. Listar clientes\n");
    printf("4. Listar compras\n");
    printf("5. Listar productos\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1; //Se verifica que valor introducido sea un numero

        switch(opcion){
            case(1):    //LISTAR REGIONES
                listarRegiones(DercoChile->regiones);
                return 0;

            case(2):    //LISTAR SEDES
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones! Por lo tanto no hay sedes\n\n");
                    return 0;
                }

                recRegion = DercoChile->regiones;

                do{
                    printf("Region: %s | Cantidad de Sedes: %d\n", recRegion->datosRegion->nombre, recRegion->datosRegion->cantSedes);
                    listarSedes(recRegion->datosRegion->sedes);
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                return 0;

            case(3):    //LISTAR CLIENTES
                listarClientes(DercoChile->clientes);
                return 0;

            case(4):    //LISTAR COMPRAS
                if(!DercoChile->clientes){
                    printf("Error. No existe lista clientes! Por lo tanto no hay compras\n\n");
                    return 0;
                }

                recCliente = DercoChile->clientes;
                do{
                    printf("Nombre cliente: %s | ID cliente: %d\n", recCliente->datosCliente->nombre, recCliente->datosCliente->idCliente);
                    listarCompras(recCliente->datosCliente->listaCompras);
                    recCliente = recCliente->sig;
                }while(recCliente != DercoChile->clientes);

                return 0;

            case(5):    //LISTAR PRODUCTOS
                if(!DercoChile->regiones){
                    printf("Error. No existen regiones! Por lo tanto no hay sedes ni productos\n\n");
                    return 0;
                }

                recRegion = DercoChile->regiones;
                do{
                    recSedes = recRegion->datosRegion->sedes;
                    printf("Region: %s | ID: %d\n", recRegion->datosRegion->nombre, recRegion->datosRegion->id);
                    if(!recRegion->datosRegion->sedes){
                        printf("Error. No hay sedes en la region! Por lo tanto no hay productos\n\n");
                    }

                    while(recSedes){
                        printf("Productos en sede ID %d:\n\n", recSedes->datosSede->idSede);

                        if(!recSedes->datosSede->stockProductos) printf("No hay productos en sede\n\n");

                        else listarProductos(recSedes->datosSede->stockProductos);

                        recSedes = recSedes->sig;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                return 0;

            case(0):
                printf("Volviendo al menu principal\n\n");
                return 0;

            default:
                printf("Error. Opcion no valida\n");
        }
    }
}

//Menu de las funciones buscar para cada struct. Si se encuentra el elemento buscado se imprimen sus datos
int menuBuscar(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoSedes *recSede;
    struct NodoClientes *recCliente;

    struct Producto *productoBuscado;
    struct Region *regionBuscada;
    struct Sede *sedeBuscada;
    struct Cliente *clienteBuscado;
    struct Compra *compraBuscada;

    int opcion, idBuscada, i;

    printf("Elija elemento a buscar:\n\n");
    printf("1. Buscar region\n");
    printf("2. Buscar sede\n");
    printf("3. Buscar un cliente\n");
    printf("4. Buscar una compra\n");
    printf("5. Buscar un producto\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion)!= 1) return 1;


        switch(opcion){
            case(1):    //BUSCAR REGION
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones!\n\n");
                    return 0;
                }

                printf("Ingrese ID region a buscar: ");
                if(scanf("%d",&idBuscada)!= 1) return 1;

                regionBuscada = buscarRegion(DercoChile->regiones, idBuscada);

                if(!regionBuscada){
                    printf("No se encontro ninguna region con ese ID\n\n");
                    return 0;
                }

                printf("Se encontro la region de ID: %d\n\n", idBuscada);
                printf("Nombre: %s\n",regionBuscada->nombre);
                printf("Cantidad sedes: %d\n\n",regionBuscada->cantSedes);

                return 0;

            case(2):    //BUSCAR SEDE
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones! Por lo tanto no hay sedes\n\n");
                    return 0;
                }

                printf("Ingrese ID de la sede a buscar: ");
                if(scanf("%d",&idBuscada) != 1) return 1;

                recRegion = DercoChile->regiones;
                do{
                    sedeBuscada = buscarSede(recRegion->datosRegion->sedes, idBuscada);

                    if(sedeBuscada){
                        printf("Se encontro la sede de ID: %d\n\n", idBuscada);
                        printf("Direccion: %s\n", sedeBuscada->direccion);
                        printf("Supervisor: %s\n\n", sedeBuscada->supervisor);
                        return 0;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                printf("No se encontro la sede\n\n");
                return 0;

            case(3):    //BUSCAR CLIENTE
                if(!DercoChile->clientes){
                    printf("Error. No existe lista clientes!\n\n");
                    return 0;
                }

                printf("Ingrese ID cliente a buscar: ");
                if(scanf("%d", &idBuscada) != 1) return 1;

                clienteBuscado = buscarCliente(DercoChile->clientes, idBuscada);

                if(!clienteBuscado){
                    printf("No se encontro cliente con ese ID\n\n");
                    return 0;
                }

                printf("Se encontro el cliente de ID: %d\n\n",idBuscada);
                printf("Nombre cliente: %s\n",clienteBuscado->nombre);
                printf("RUT: %s\n\n",clienteBuscado->rut);

                return 0;

            case(4):    //BUSCAR COMPRA
                if(!DercoChile->clientes){
                    printf("Error. No existe lista clientes! Por lo tanto no hay compras\n\n");
                    return 0;
                }

                printf("Ingrese ID compra a buscar: ");
                if(scanf("%d",&idBuscada) != 1) return 1;

                recCliente = DercoChile->clientes;
                do{
                    compraBuscada = buscarCompra(recCliente->datosCliente->listaCompras, idBuscada);

                    if(compraBuscada){
                        printf("Se encontro la compra de ID: %d\n\n",idBuscada);
                        printf("Fecha de la compra: %s\n", compraBuscada->fecha);
                        printf("Monto total compra: $%d\n", compraBuscada->montoTotal);
                        printf("ID Sede en la que se realizo compra: %d\n\n",compraBuscada->SedeCompra->idSede);
                        printf("Cantidad de productos comprados: %d\n", compraBuscada->cantProductosComprados);
                        printf("IDs productos comprados:\n");

                        for(i = 0 ; i<(compraBuscada->cantProductosComprados)-1 ; i++){
                            printf("%d - ",compraBuscada->productosComprados[i]->id);
                        }
                        printf("%d",compraBuscada->productosComprados[i]->id);
                        printf("\n\n");
                        return 0;
                    }
                    recCliente = recCliente->sig;
                }while(recCliente != DercoChile->clientes);

                printf("No se encontro la compra\n\n");
                return 0;

            case(5):    //BUSCAR PRODUCTO
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones! Por lo tanto no hay sedes ni productos\n\n");
                    return 0;
                }

                printf("Ingrese ID producto a buscar: ");
                if(scanf("%d", &idBuscada) != 1) return 1;

                recRegion = DercoChile->regiones;

                do{
                    if(recRegion->datosRegion->sedes){
                        recSede = recRegion->datosRegion->sedes;

                        while(recSede){
                            productoBuscado = buscarProducto(recSede->datosSede->stockProductos, idBuscada);
                            if(productoBuscado){
                                printf("Se encontro producto de ID: %d\n\n", idBuscada);

                                printf("Marca: %s\n", productoBuscado->marca);
                                if(productoBuscado->modelo){
                                    printf("Modelo: %s\n", productoBuscado->modelo);
                                    printf("Cantidad de recalls: %d\n", productoBuscado->cantRecalls);
                                }
                                else{
                                    printf("Nombre: %s\n", productoBuscado->nombre);
                                }
                                printf("Precio: $%d\n\n",productoBuscado->precio);

                                return 0;
                            }
                            recSede = recSede->sig;
                        }
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                printf("No se encontro producto con esa ID\n\n");
                return 0;

            case(0):
                return 0;

            default:
                printf("Error. Opcion no valida. Introduzca opcion valida\n");
        }
    }
}

//Menu de funciones de agregar para cada tipo de struct. En cada caso primero se crea el struct a agregar y despues se agrega a la lista/ABB
int menuAgregar(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct Region *nuevaRegion, *regionAgregar;

    struct Sede *nuevaSede, *sedeAgregar;
    struct NodoSedes *recSede;

    struct Producto *nuevoProducto;
    struct Cliente *nuevoCliente, *clienteAgregar;

    struct NodoClientes *recCliente;
    struct Compra *nuevaCompra;

    int opcion, idAgregar;

    printf("Elija elemento a agregar:\n\n");
    printf("1. Agregar region\n");
    printf("2. Agregar sede\n");
    printf("3. Agregar un cliente\n");
    printf("4. Agregar una compra\n");
    printf("5. Agregar un producto\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1;

        switch(opcion){
            case(1):    //AGREGAR REGION
                printf("Ingrese datos de la nueva region:\n\n");
                nuevaRegion = crearRegion();

                if(!nuevaRegion){
                    printf("Error. Valor introducido no es valido\n\n");
                    return 0;
                }

                if(agregarRegion(&(DercoChile->regiones), nuevaRegion))
                    printf("Se agrego la region correctamente\n\n");

                else{
                    printf("Error. ID region ya existe\n");
                    printf("No se puedo agregar la region\n\n");
                }

                return 0;

            case(2):    //AGREGAR SEDE
                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una region primero\n\n");
                    return 0;
                }

                printf("Ingrese ID de region donde agregar:");
                scanf("%d",&idAgregar);

                regionAgregar = buscarRegion(DercoChile->regiones, idAgregar);

                if(!regionAgregar){
                    printf("Error. Region no existe\n\n");
                    return 0;
                }

                printf("Ingrese datos de la nueva sede:\n\n");
                nuevaSede = crearSede();

                if(!nuevaSede){
                    printf("Error. Valor introducido no es valido.\n\n");
                    return 0;
                }

                recRegion = DercoChile->regiones;

                do{
                    if(buscarSede(recRegion->datosRegion->sedes, nuevaSede->idSede)){
                        printf("Error. Ya existe Sede con esa ID\n");
                        printf("No se pudo agregar la Sede\n\n");
                        return 0;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                if(agregarSede(&(regionAgregar->sedes), nuevaSede)){
                    regionAgregar->cantSedes += 1;
                    printf("Se agrego la sede correctamente\n\n");
                }

                else{
                    printf("No se pudo agregar la sede\n\n");
                }
                return 0;

            case(3):    //AGREGAR CLIENTE
                printf("Ingrese datos del nuevo cliente:\n\n");
                nuevoCliente = crearCliente();

                if(!nuevoCliente){
                    printf("Error. ID no valido.\n\n");
                    return 0;
                }

                if(agregarCliente(&(DercoChile->clientes), nuevoCliente))
                    printf("Se agrego el cliente correctamente\n\n");

                else{
                    printf("Error. ID cliente ya existe\n");
                    printf("No se pudo agregar cliente\n\n");
                }

                return 0;

            case(4):    //AGREGAR COMPRA

                if(!DercoChile->clientes){
                    printf("Error. Necesita agregar un cliente primero\n\n");
                    return 0;
                }

                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una region primero\n\n");
                    return 0;
                }

                printf("Ingrese ID de cliente que realiza la compra:");
                scanf("%d",&idAgregar);

                clienteAgregar = buscarCliente(DercoChile->clientes, idAgregar);

                if(!clienteAgregar){
                    printf("Error. Cliente no existe\n\n");
                    return 0;
                }

                printf("Ingrese datos de la nueva compra:\n\n");
                printf("Ingrese ID Sede en la que se realizo compra:");

                if(scanf("%d",&idAgregar) != 1) return 1;

                recRegion = DercoChile->regiones;
                do{
                    sedeAgregar = buscarSede(recRegion->datosRegion->sedes, idAgregar);
                    if(sedeAgregar) break;
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                if(!sedeAgregar){
                    printf("Error. No existe sede de ID %d\n\n", idAgregar);
                    return 0;
                }

                nuevaCompra = crearCompra(sedeAgregar);

                if(!nuevaCompra){
                    printf("Error. Sede no tiene productos!\n\n");
                    return 0;
                }

                recCliente = DercoChile->clientes;
                do{
                    if(buscarCompra(recCliente->datosCliente->listaCompras, nuevaCompra->idBoleta)){
                        printf("Ya existe compra con ese ID\n");
                        printf("No se pudo agregar la compra\n\n");
                        return 0;
                    }
                    recCliente = recCliente->sig;
                }while(recCliente != DercoChile->clientes);


                if(agregarCompra(&(clienteAgregar->listaCompras), nuevaCompra)){
                    printf("Se agrego la compra correctamente\n\n");
                }

                else{
                    printf("No se pudo agregar la compra\n\n");
                }

                return 0;

            case(5):    //PRODUCTO
                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una region y sede primero\n");
                    return 0;
                }

                printf("Ingrese ID de la sede donde agregar: ");
                scanf("%d", &idAgregar);

                recRegion = DercoChile->regiones;
                do{
                    sedeAgregar = buscarSede(recRegion->datosRegion->sedes, idAgregar);
                    if(sedeAgregar) break;

                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                if(!sedeAgregar){
                    printf("Error. Sede no existe!\n\n");
                    return 0;
                }

                printf("Ingrese datos del nuevo producto:\n");
                printf("Si producto no es un auto escribir '.' en campo de Modelo\n\n");
                nuevoProducto = crearProducto();

                recRegion = DercoChile->regiones;

                do{
                    recSede = recRegion->datosRegion->sedes;
                    while(recSede){
                        if(buscarProducto(recSede->datosSede->stockProductos, nuevoProducto->id)){
                            printf("Error. Ya existe producto con esa ID\n\n");
                            return 0;
                        }
                        recSede = recSede->sig;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                agregarProducto(&(sedeAgregar->stockProductos), nuevoProducto);

                return 0;

            case(0):
                return 0;

            default:
                printf("Error. Opcion no valida. Introduzca opcion valida\n");
        }
    }
}

//Menu de funciones de eliminar un elemento para cada tipo de struct.
int menuEliminar(struct SistemaDerco *DercoChile){
    struct NodoRegion *recRegion;
    struct NodoClientes *recCliente;
    struct NodoSedes *recSede;
    int opcion, idEliminar;

    printf("Elija elemento a eliminar:\n\n");
    printf("1. Eliminar region\n");
    printf("2. Eliminar sede\n");
    printf("3. Eliminar un cliente\n");
    printf("4. Eliminar una compra\n");
    printf("5. Eliminar un producto\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1;

        switch(opcion){
            case(1):    //ELIMINAR REGION
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones!\n\n");
                    return 0;
                }

                printf("Ingrese ID region a eliminar: ");
                if(scanf("%d", &idEliminar) != 1) return 1;

                if(eliminarRegion(&(DercoChile->regiones),idEliminar))
                    printf("Se elimino la region correctamente\n\n");

                else
                    printf("No se pudo eliminar la region\n\n");

                return 0;

            case(2):    //ELIMINAR SEDE
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones! Por lo tanto no hay sedes\n\n");
                    return 0;
                }

                printf("Ingrese ID de la sede a eliminar: ");
                if(scanf("%d", &idEliminar) != 1) return 1;

                recRegion = DercoChile->regiones;

                do{
                    if(eliminarSede(&(recRegion->datosRegion->sedes), idEliminar)){
                        recRegion->datosRegion->cantSedes -= 1;
                        printf("Se elimino la sede correctamente\n\n");
                        return 0;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);


                printf("No se pudo eliminar la sede\n\n");
                return 0;

            case(3):    //ELIMINAR CLIENTE
                if(!DercoChile->clientes){
                    printf("Error. No existe lista de clientes!\n\n");
                    return 0;
                }

                printf("Ingrese ID cliente a eliminar: ");
                if(scanf("%d", &idEliminar) != 1) return 1;

                if(eliminarCliente(&(DercoChile->clientes), idEliminar))
                    printf("Se elimino cliente correctamente\n\n");

                else
                    printf("No se pudo eliminar el cliente\n\n");

                return 0;

            case(4):    //ELIMINAR COMPRA
                if(!DercoChile->clientes){
                    printf("Error. No existe lista clientes! Por lo tanto no hay compras\n\n");
                    return 0;
                }

                printf("Ingrese ID de la compra a eliminar: ");
                if(scanf("%d", &idEliminar) != 1) return 1;

                recCliente = DercoChile->clientes;

                do{
                    if(buscarCompra(recCliente->datosCliente->listaCompras, idEliminar)) break;
                    recCliente = recCliente->sig;
                }while(recCliente != DercoChile->clientes);

                if(eliminarCompra(&(recCliente->datosCliente->listaCompras), idEliminar)){
                    printf("Se elimino compra correctamente\n\n");
                    return 0;
                }

                printf("No se pudo eliminar la compra\n\n");
                return 0;

            case(5):    //ELIMINAR PRODUCTO
                if(!DercoChile->regiones){
                    printf("Error. No existe lista regiones! Por lo tanto no hay sedes ni productos\n\n");
                    return 0;
                }

                printf("Ingrese ID del producto a eliminar: ");
                if(scanf("%d", &idEliminar) != 1) return 1;


                recRegion = DercoChile->regiones;

                do{
                    recSede = recRegion->datosRegion->sedes;
                    while(recSede){
                        if(buscarProducto(recSede->datosSede->stockProductos, idEliminar)){
                            eliminarProducto(&(recSede->datosSede->stockProductos), idEliminar);
                            printf("Se elimino producto correctamente\n\n");
                            return 0;
                        }
                        recSede = recSede->sig;
                    }
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                printf("Error. No se pudo eliminar el producto\n\n");
                return 0;

            case(0):
                return 0;

            default:
                printf("Error. Opcion no valida. Introduzca opcion valida\n");
        }
    }
}

//Menu para modificar cada tipo de struct
int menuModificar(struct SistemaDerco *DercoChile){
    int opcion, idModificar;
    struct NodoRegion *recRegion;
    struct NodoClientes *recCliente;
    struct NodoSedes *recSede;

    struct Sede *sedeModificar;
    struct Region *regionModificar;
    struct Cliente *clienteModificar;
    struct Compra *compraModificar;
    struct Producto *productoModificar;

    printf("Elija elemento a Modificar:\n\n");
    printf("1. Modificar region\n");
    printf("2. Modificar sede\n");
    printf("3. Modificar un cliente\n");
    printf("4. Modificar una compra\n");
    printf("5. Modificar un producto\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1;

        switch(opcion){
            case(1): //MODIFICAR REGION
                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una region primero\n\n");
                    return 0;
                }

                printf("Ingrese ID region a modificar: ");
                if(scanf("%d", &idModificar) != 1) return 1;


                regionModificar = buscarRegion(DercoChile->regiones, idModificar);

                if(!regionModificar){
                    printf("Error. No existe region con ese ID\n\n");
                    return 0;
                }

                printf("Introduzca los nuevos valores. \nEn caso de no realizar cambios introduzca 0 para valores numericos y '.' para cadenas\n\n");


                if(modificarRegion(DercoChile->regiones, regionModificar))
                    printf("Se modifico la region correctamente\n\n");

                else
                    printf(" No se pudo modificar la region correctamente\n\n");

                return 0;

            case(2):    //MODIFICAR SEDE
                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una sede primero\n\n");
                    return 0;
                }

                printf("Ingrese ID sede a modificar: ");
                if(scanf("%d", &idModificar)!=1) return 1;

                recRegion = DercoChile->regiones;

                do{
                    sedeModificar = buscarSede(recRegion->datosRegion->sedes, idModificar);
                    if(sedeModificar) break;
                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                if(!sedeModificar){
                    printf("Error. No existe sede con esa ID\n\n");
                    return 0;
                }

                printf("Introduzca los nuevos valores. \nEn caso de no realizar cambios introduzca 0 para valores numericos y '.' para cadenas\n\n");
                if(modificarSede(DercoChile->regiones, sedeModificar))
                    printf("Se modifico la sede correctamente\n\n");

                else
                    printf(" No se pudo modificar la sede correctamente\n\n");

                return 0;

            case(3):    //MODIFICAR CLIENTE
                if(!DercoChile->clientes){
                    printf("Error. Necesita agregar un cliente primero\n\n");
                    return 0;
                }

                printf("Ingrese ID cliente a modificar: ");
                if(scanf("%d", &idModificar) != 1) return 1;

                clienteModificar = buscarCliente(DercoChile->clientes, idModificar);

                if(!clienteModificar){
                    printf("Error. No existe cliente con ese ID\n\n");
                    return 0;
                }

                printf("Introduzca los nuevos valores. \nEn caso de no realizar cambios introduzca 0 para valores numericos y '.' para cadenas\n\n");
                if(modificarCliente(DercoChile->clientes,clienteModificar))
                    printf("Se modifico cliente correctamente\n\n");

                else
                    printf(" No se pudo modificar cliente correctamente\n\n");

                return 0;

            case(4): //MODIFICAR COMPRA

                if(!DercoChile->clientes){
                    printf("Error. Necesita agregar una compra primero\n\n");
                    return 0;
                }

                printf("Ingrese ID compra a modificar: ");
                if(scanf("%d", &idModificar)!=1) return 1;

                recCliente = DercoChile->clientes;

                do{
                    compraModificar = buscarCompra(recCliente->datosCliente->listaCompras, idModificar);
                    if(compraModificar) break;
                    recCliente = recCliente->sig;
                }while(recCliente != DercoChile->clientes);

                if(!compraModificar){
                    printf("Error. No existe compra con ese ID\n\n");
                    return 0;
                }

                printf("Introduzca los nuevos valores. \nEn caso de no realizar cambios introduzca 0 para valores numericos y '.' para cadenas\n\n");
                if(modificarCompra(DercoChile->clientes, compraModificar))
                    printf("Se modifico la compra correctamente\n\n");

                else
                    printf(" No se pudo modificar la compra correctamente\n\n");

                return 0;

            case(5): //MODIFICAR PRODUCTO
                if(!DercoChile->regiones){
                    printf("Error. Necesita agregar una region primero\n\n");
                    return 0;
                }

                printf("Ingrese ID producto a modificar:");
                if(scanf("%d",&idModificar) != 1) return 1;

                recRegion = DercoChile->regiones;
                do{
                    recSede = recRegion->datosRegion->sedes;

                    while(recSede){
                        productoModificar = buscarProducto(recSede->datosSede->stockProductos, idModificar);
                        if(productoModificar) break;
                        recSede = recSede->sig;
                    }

                    if(productoModificar) break;

                    recRegion = recRegion->sig;
                }while(recRegion != DercoChile->regiones);

                if(!productoModificar){
                    printf("No existe producto con esa ID\n\n");
                    return 0;
                }

                printf("Introduzca los nuevos valores. \nEn caso de no realizar cambios introduzca 0 para valores numericos y '.' para cadenas\n\n");

                modificarProducto(recSede->datosSede->stockProductos, productoModificar);
                printf("Se modifico producto con exito\n\n");

                return 0;

            case(0):
                return 0;

            default:
                printf("Error. Opcion no valida. Introduzca opcion valida\n");
        }
    }
}

//Menu que se divide en submenus para cada una de las operaciones basicas
int menuOperacionesBasicas(struct SistemaDerco *DercoChile){
    int opcion;

    printf("Operaciones basicas:\n\n");
    printf("1. Listar un elemento\n");
    printf("2. Buscar un elemento\n");
    printf("3. Agregar un elemento\n");
    printf("4. Eliminar un elemento\n");
    printf("5. Modificar un elemento\n");
    printf("\n0. Volver al menu principal\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1;

        switch(opcion){
            case 1:         //MENU LISTAR
                if(menuListar(DercoChile)) return 1;
                return 0;

            case 2:         //MENU BUSCAR
                if(menuBuscar(DercoChile)) return 1;
                return 0;

            case 3:         //MENU AGREGAR
                if(menuAgregar(DercoChile)) return 1;
                return 0;

            case 4:         //MENU ELIMINAR
                if(menuEliminar(DercoChile)) return 1;
                return 0;

            case 5:         //MENU MODIFICAR
                if(menuModificar(DercoChile)) return 1;
                return 0;

            case 0:
                return 0;

            default:
                printf("Error. Opcion no es valida. Introduzca opcion valida\n");
                break;
        }
    }
}

//Menu principal, desde el cual se accede al menu de operaciones basicas o se llama a alguna de las funciones adicionales
int menuPrincipal(struct SistemaDerco *DercoChile){
    struct Cliente *cliente;
    struct Sede *sede, **sedesDeficientes;
    struct Producto *autoMasRecalls;

    int opcion, i;

    printf("MENU PRINCIPAL\n");
    printf("Elija operacion a realizar:\n\n");
    printf("1. Operaciones basicas\n");
    printf("2. Modelos y marcas de autos vendidos por region\n");
    printf("3. Cliente que mas dinero ha gastado\n");
    printf("4. Sede con mayor distribucion\n");
    printf("5. Auto con mas recalls\n");
    printf("6. Sedes con ventas bajo el promedio\n");
    printf("7. Monto total de dinero recaudado en todo Chile\n");
    printf("8. Imprimir datos empresa\n");
    printf("\n0. Salir del programa\n\n");

    while(1){
        if(scanf("%d",&opcion) != 1) return 1;

        switch(opcion){
            case(1):        //Menu operaciones basicas
                if(menuOperacionesBasicas(DercoChile)) return 1;
                return 0;

            case(2):        //Modelos y marcas de autos vendidos por region en todo el pais
                if(!DercoChile->regiones){
                    printf("Error. No hay regiones registradas, por lo tanto no hay sedes ni productos\n\n");
                    return 0;
                }

                modelosMarcasVendidosPorZona(DercoChile);
                return 0;

            case(3):		//Cliente que mas dinero ha gastado
                if(!DercoChile->clientes){
                    printf("Error. No hay clientes registrados!\n\n");
                    return 0;
                }

                cliente = clienteQueMasGasta(DercoChile);

                if(contarDineroGastado(cliente) == 0){
                    printf("Error. No hay compras registradas!\n\n");
                    return 0;
                }

                printf("Nombre cliente: %s\n", cliente->nombre);
                printf("RUT %s\n", cliente->rut);
                printf("ID cliente: %d\n", cliente->idCliente);

                printf("Total de dinero gastado: %d\n\n", contarDineroGastado(cliente));

                return 0;

            case(4):		//Sede con mayor distribucion
                if(!DercoChile->regiones){
                    printf("Error. No hay regiones agregadas, por lo tanto no hay sedes\n\n");
                    return 0;
                }

                if(!DercoChile->clientes){
                    printf("Error. No hay clientes registrados, por lo tanto no hay ventas\n\n");
                    return 0;
                }

                sede = sedeMayorDistribucion(DercoChile);

                if(sede == NULL){
                    printf("Error. No hay ventas registradas!\n\n");
                    return 0;
                }
                
                
				printf("Cantidad de productos vendidos en la sede: %d\n\n",contadorCantVentasSede(sede->idSede, DercoChile->clientes));
                printf("ID Sede %d\n", sede->idSede);
                printf("Direccion %s\n", sede->direccion);
                printf("Supervisor %s\n\n", sede->supervisor);

                return 0;

            case(5):		//Auto con mas recalls
                if(!DercoChile->regiones){
                    printf("Error. No hay regiones agregadas, por lo tanto no hay sedes ni autos\n\n");
                    return 0;
                }

                autoMasRecalls = autoConMasRecalls(DercoChile);

                if(autoMasRecalls == NULL){
                    printf("Error. No hay autos con recalls\n\n");
                    return 0;
                }

                printf("ID Auto: %d\n", autoMasRecalls->id);
                printf("Modelo: %s\n", autoMasRecalls->modelo);
                printf("Cantidad de recalls %d\n\n", autoMasRecalls->cantRecalls);

                return 0;

            case(6):			//Sedes deficientes
                if(!DercoChile->regiones){
                    printf("Error. No hay regiones agregadas, por lo tanto no hay sedes\n\n");
                    return 0;
                }

                sedesDeficientes = sedesBajoPromedioVentas(DercoChile);

                printf("Promedio de ventas por sede: $%.f\n", calcularPromedioVentas(DercoChile));
                if(!sedesDeficientes){
                    printf("No hay sedes deficientes\n\n");
                    return 0;
                }

                printf("Sedes bajo el promedio:\n\n");
                for(i = 0 ; i< cantidadSedesBajoPromedio(DercoChile) ; i++){
                    printf("ID Sede: %d | Direccion: %s | Supervisor: %s\n", sedesDeficientes[i]->idSede, sedesDeficientes[i]->direccion, sedesDeficientes[i]->supervisor);
                }
                printf("\n");

                return 0;

            case(7):		//Total de dinero recaudado
                if(!DercoChile->clientes){
                    printf("Error. No hay clientes agregados, por lo tanto no hay compras registradas\n\n");
                    return 0;
                }

                dineroRecaudado(DercoChile);
                return 0;

            case(8):		//Datos de la empresa
                mostrarDatosEmpresa(DercoChile);
                return 0;

            case(0):		//Opcion para terminar el programa
                return -1;

            default:
                printf("Error. Opcion no valida. Introduzca opcion valida\n");
                break;
        }
    }
}

int main(){
    struct SistemaDerco *DercoChile;
    int salida;

    printf("Inicio programa\n\n");

    DercoChile = (struct SistemaDerco*) malloc(sizeof(struct SistemaDerco));

    DercoChile->ceo = (char*) malloc (strlen("Romeo Lacerda")*sizeof(char));
    strcpy((DercoChile)->ceo,"Romeo Lacerda");

    DercoChile->rutEmpresarial = (char*)malloc(sizeof(char)*strlen("82.995.700-0"));
    strcpy((DercoChile)->rutEmpresarial,"82.995.700-0");

    DercoChile->cantEmpleados = 4400;

    DercoChile->regiones = NULL;
    DercoChile->clientes = NULL;

    while(1){
        salida = menuPrincipal(DercoChile);

        if(salida == 1){
            printf("Error. Opcion no es un valor numerico. Cerrando programa\n");
            return 1;
        }

        if(salida == -1){
            printf("Cerrando programa\n");
            return 0;
        }
    }
}
