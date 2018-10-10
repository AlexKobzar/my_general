#ifndef SUPPLY_H   
#define SUPPLY_H     


// ****************   Управление питанием каскадов:
#bit EN1 = bufPortA.3   // пины разрешения питания каскадов
#bit EN2 = bufPortA.0
#bit EN3 = bufPortB.7
#bit EN4 = bufPortB.6
#bit EN5 = bufPortB.5
#bit EN6 = bufPortB.4

uint8_t   en_state;

#bit EN1_STATE = en_state.0
#bit EN2_STATE = en_state.1
#bit EN3_STATE = en_state.2
#bit EN4_STATE = en_state.3
#bit EN5_STATE = en_state.4
#bit EN6_STATE = en_state.5

#inline void supply_manager (char num, char state);
#inline void supply_emergency_shutdown();

//************   Коммутатор  HMC284AMS8G  **********************************
void commutator_set (uint8_t sw);
void commutator_manager (char state);




#endif
