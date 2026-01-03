#ifndef GRADODIFFICOLTA_H
#define GRADODIFFICOLTA_H
/*
GradoDifficolta:
   - rappresenta lo "stato" dell'immobile/cantiere (nuovo, disabitato, abitato)
   - è un'informazione di dominio: serve per calcolare un costo diverso a parità di mq
*/
enum class GradoDifficolta {
    Nuovo,       //appartamento nuovo / vuoto, situazione più semplice
    Disabitato,  //casa disabitata quindi permette i lavori ma con delle complicazioni come possono essere mobili già installati
    Abitato      //casa abitata: più complicazioni, coperture, spostamenti ecc.
};
#endif
