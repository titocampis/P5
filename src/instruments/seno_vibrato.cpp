#include <iostream>
#include <math.h>
#include "seno_vibrato.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

//El modo sencillo de generar un sonido periodico de frecuencia
//variable es almacenar un periodo del mismo en una tabala y 
//recorrer esta a una velocidad adecuada para conseguir
//el pitch deseado

//Constructor
seno_vibrato::seno_vibrato(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */

  //Usando funciones de la libreria keyvalue.h analizamos
 //la cadena de parámetros de interes para el instrumento, en este caso
 //solamente la longitud de la tabla N, ya que los parametros ADSR se procesan 
 //en otra parte
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value

  if (!kv.to_float("N1",N1))
    N1 = 4; //default value

  if (!kv.to_float("N2",N2))
    N2 = 1; //default value

  if (!kv.to_float("I",I))
    I = 0.000328; //default value
  
  //Se crea la tabla con un periodo de sinusoide
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}

// Cada vez que el programa encuentra un comando MIDI en el fichero score, invoca al
// método command(comando, la nota, la velocidad)
// si el comando es 9 (NoteOn) se declara activo el instrumento
// inicializa curva ADSR y un contador index que recorre la tabla,
// de amplitud A. Si cmd = 8 o 0 (NoteOff o EndNote) el metoo inicia la fase release de la curva
// o fiinaliza el sonido, respectivament
void seno_vibrato::command(long cmd, long note, long vel) {
  // NoteOn instrumento Activo
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;
    // Sabiendo la correspondencia de La central definimos F0 para otras notas
    // También definimos las relaciones con la frecuencia central de la portadora 
    // y de la moduladora
    float F0 = 440.00 * pow(2, (note - 69.00)/12.00) / SamplingRate;
    float Fc = N1 * F0;
    float Fm = N2 * F0;

    // Definimos el salto como antes y configuaramos
    step = tbl.size() * Fm;
    phase_m = 0;
    alpha = 2 * M_PI * Fc;
  }
  //Fase release de la curva
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  //Finaliza el sonido
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

//La sintesis se realiza aquí
//La nota puede encontrarse en 3 situaciones Finalizada, Inactiva, Activa:
const vector<float> & seno_vibrato::synthesize() {
  //Finalizada: si la curva ha llegado a su nivel final
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  //Inactiva si ya ha sido marcada como inactiva
  else if (not bActive)
    return x;
  //Activa, se aplica la envolvente ADSR, aquí es donde se recorre la tabla
  for (unsigned int i=0; i<x.size(); ++i) {
    // Primera aproximación: redondeando el indice requerido para acceder a valores de la tabla
    float new_index = round(index * step);
    x[i] = A * (tbl[new_index]);
    index += (1 + I * sin(phase_m)/step);
    phase_m += alpha;
    if (new_index == tbl.size()) index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
