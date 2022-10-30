#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
using namespace std;


int count(int number, int array[], int length) {
    if (length == 0) return 0;
    return (number == *array) + count(number, array+1, length-1);
}

int main(int argc, char** argv){
    if (argc > 2)
    {
        cerr << "Please provide a filename correctly\n";
        exit(1);   // call system to stop
    }
    
    int x;
    int y;
    srand(time(NULL));

    ifstream file_reader;
    string filename = argv[1];
    filename += ".txt";
    
    file_reader.open(filename);
    if (!file_reader) {
        cerr << "Unable to open " + filename;
        exit(1);   // call system to stop
    }
    file_reader >> x;
    int total_demand = x;
    //cout << total_demand << endl;
    file_reader >> x;
    int n_options = x;
    //cout << n_options << endl;
    file_reader >> x;
    int n_classes = x;
    int op[n_options][2];
    int car_classes[n_classes][n_options];
    int i = 0;
    while (i < n_options)
    {
        file_reader >> op[i][0];
        //cout << op[i][0] << endl;
        i++;
    }
    i = 0;
    while (i < n_options)
    {
        file_reader >> op[i][1];
        //cout << op[i][1] << endl;
        i++;
    }
    int class_demand[n_classes];
    for (size_t j = 0; j < n_classes; j++)
    {
        if( file_reader.eof() ) break;
        file_reader >> x;
        file_reader >> y;
        class_demand[j] = y;
        //cout << "demanda de " << j << " " << class_demand[j] << endl;
        for (size_t k = 0; k < n_options; k++)
        {
            file_reader >> car_classes[j][k];
            //cout << car_classes[j][k] << endl;
        }
    }
    /*n_classes n_options class_demand[n_classes] car_classes[n_classes][n_options] op[n_option][2]*/
    int Slots[total_demand];
    int lista_variable_conectada_mas_reciente[total_demand];

    for (size_t i = 0; i < total_demand; i++)
    {
        Slots[i] = -1;
        lista_variable_conectada_mas_reciente[i] = 0; //almacena la variable mas recientemente conectada de i
    }
    file_reader.close();

    bool exit = false;
    int suma_capacidades; 
    int inicio_loop = 0;

    //bool plazo_de_extension = false;

    int total_de_faltas = 0;

    const clock_t begin_time = clock();
    clock_t begin_time_2 = clock();

    int temp = 0;
    for (size_t i = 0; i < n_classes; i++)
    {
        temp += class_demand[i];
    }
    if (temp != total_demand)
    {
        cerr << "Las demandas individuales no suman la demanda total\n";
        exit;
    }
    
    
    
    cout<<"Empezando..."<<endl;

    back:
    for (size_t i = 0; i < total_demand; i++)
    {
        if (Slots[i] == -1) //revisamos el primer elemento con -1 (clase no asignada)
        {
            //lista_variable_conectada_mas_reciente[i] = 0; // para futuras comparaciones
            //inicio_loop = 0;
            for (size_t j = inicio_loop; j < n_classes; j++)
            {
                if (float( clock () - begin_time_2 ) /  CLOCKS_PER_SEC > 60)
                {
                    cout<<"Still Alive... "<<i<<" "<<lista_variable_conectada_mas_reciente[i]<<endl;
                    begin_time_2 = clock();
                }

                if (count(j,Slots,total_demand) < class_demand[j])
                {
                    Slots[i] = j;
                }else
                {
                    if (j == n_classes-1)
                    {
                        salto:
                        while (true)
                        {
                            if (Slots[lista_variable_conectada_mas_reciente[i]] == n_classes -1)// || (Slots[lista_variable_conectada_mas_reciente[i]] == n_classes -2 && count(n_classes-1,Slots,total_demand) == class_demand[n_classes-1]))
                            {
                                i = lista_variable_conectada_mas_reciente[i];
                            }else
                            {
                                //cout<<"salto a "<<lista_variable_conectada_mas_reciente[i]<<endl;
                                inicio_loop = Slots[lista_variable_conectada_mas_reciente[i]] + 1;
                                int temp = lista_variable_conectada_mas_reciente[i];
                                for (size_t p = temp; p < total_demand; p++)
                                {
                                    Slots[p] = -1;
                                }
                                goto back;
                            }
                        }
                    }else
                    {
                        continue;
                    }
                }
                /*
                
                if (class_demand[j]==count(j,Slots,total_demand))
                {                  
                    debemos instanciar otra variable, 
                    problemas con el dominio de la variable 
                    que actualmente queremos intanciar
                    if (j==n_classes-1)
                    {
                        inicio_loop = Slots[i-1] + 1;
                        Slots[i-1] = -1;
                        cout<<"caso especial"<<endl;
                        goto back;
                    }
                      
                    continue;
                }
                
                Slots[i] = j; //le asignamos una clase*/
                //ahora revisamos si existe conflicto
                for (size_t k = 0; k < n_options; k++)
                {
                    //revisamos capacidades de cada opcion k que la clase j usa
                    if (car_classes[j][k] == 1)
                    {
                        //revisamos para atras que se cumplan las capacidades en la ventana 
                        //para cada opcion k
                        //quizas el Q es muy grande, debemos revisar que no se vaya sobre el item 0 del array
                        suma_capacidades = 1;
                        for (size_t l = std::max(0, int(i-op[k][1]) + 1); l < i; l++) 
                        {
                            //si la clase que esta en el slot l usa la opcion k sumamos al contador
                            if (car_classes[Slots[l]][k] == 1)
                            {
                                //necesitamos la variable conectada mas recientemente instanciada revisando 
                                //todas las opciones de la clase j usada en el slot i
                                if (lista_variable_conectada_mas_reciente[i] < l)
                                {
                                    lista_variable_conectada_mas_reciente[i] = l;
                                }
                                suma_capacidades++;
                            }
                        }
                        //sumamos 1 para mayor flexibilidad, 
                        //menos tiempo de computo pero no encuentra solucion exacta
                        if (suma_capacidades > op[k][0] + 1 )//+ 1*(rand()<0.3)) //falta a las capacidades
                        {
                            //cout<<"falta a las capacidades"<<endl;
                            //cambiar de clase en el slot i
                            if (j == n_classes - 1) //debe entrar tambien considerando demanda
                            {
                                //aca debemos hacer un while true encontrando la
                                //primera variable que todavia tenga variables por asignar
                                //y luego hacemos todo lo necesario
                                //cout<<"no quedan variables por asignar para "<<i<<endl;
                                goto salto;
                                /*
                                while (true)
                                {
                                    
                                    if (Slots[lista_variable_conectada_mas_reciente[i]] == n_classes -1)// || (Slots[lista_variable_conectada_mas_reciente[i]] == n_classes -2 && count(n_classes-1,Slots,total_demand) == class_demand[n_classes-1]))
                                    {
                                        i = lista_variable_conectada_mas_reciente[i];
                                    }else
                                    {
                                        //cout<<"salto a "<<lista_variable_conectada_mas_reciente[i]<<endl;
                                        inicio_loop = Slots[lista_variable_conectada_mas_reciente[i]] + 1;
                                        int temp = lista_variable_conectada_mas_reciente[i];
                                        for (size_t p = temp; p < total_demand; p++)
                                        {
                                            Slots[p] = -1;
                                            lista_variable_conectada_mas_reciente[p] = 0;
                                        }
                                        if (inicio_loop == n_classes-1 && count(n_classes-1,Slots,total_demand)== class_demand[n_classes-1])
                                        {
                                            cout<<"SERIOS PROBLEMAS"<<endl;
                                        }
                                        
                                        goto back;
                                        
                                    }
                                    
                                }*/

                            }
                            exit = true;
                            break;
                        }
                    }
                }
                if (exit)
                {
                    exit = false;
                    continue;
                }
                //en el caso de que no se presenta ninguna falta a las capacidades
                inicio_loop = 0;
                //goto back1;
                break;
            }
        }
    }
    float tiempo_ejecucion = float( clock () - begin_time ) /  CLOCKS_PER_SEC;

    ofstream myfile;
    string filename_out = argv[1];
    filename_out += ".out";
    myfile.open (filename_out);
    //myfile << "Writing this to a file.\n";
    
    cout << "solucion: " << endl;
    for (size_t i = 0; i < total_demand; i++)
    {
        cout << Slots[i] << endl;
        myfile <<std::to_string(Slots[i]) + " ";
    }
    myfile<<"\n";
    //calcular total de faltas
    int faltas_por_opcion;
    int lista_faltas_por_opcion[n_options];
    for (size_t j = 0; j < n_options; j++)
    {
        faltas_por_opcion = 0;
        for (size_t i = 0; i < total_demand; i++)
        {
            suma_capacidades = 0;
            if (car_classes[Slots[i]][j] == 1)
            {
                for (size_t l = std::max(0, int(i-op[j][1]) + 1); l <= i; l++) 
                {
                    if (car_classes[Slots[l]][j] == 1)
                    {
                        suma_capacidades++;
                    }
                }
                if (suma_capacidades > op[j][0])
                {
                    total_de_faltas++;
                    faltas_por_opcion++;
                }
            }
        }
        lista_faltas_por_opcion[j] = faltas_por_opcion;
    }

    myfile<<std::to_string(total_de_faltas)+"\n";
    for (size_t i = 0; i < n_options; i++)
    {
        myfile<<std::to_string(i)+" "+std::to_string(lista_faltas_por_opcion[i])+"\n";
    }
    myfile<<std::to_string(tiempo_ejecucion);
    cout<<"Total de faltas: "<<total_de_faltas<<endl;
    std::cout <<"Tiempo empleado en GBJ: " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC<<endl;
    myfile.close();
    return 0;
}
