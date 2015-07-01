//Filipe Peracchi Pisoni - Jonas Flores

#define GLEW_STATIC
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/audiere.h"


// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
    using namespace glm;

#include "common/shader.cpp"
#include "common/texture.cpp"
#include "common/objloader.cpp"

#include "planeTexture.c"
#include "skin.c"
#include "brickTexture.c"
#include "trackTexture.c"
#include "newTrackTexture.c"
#include "sandTexture.c"
#include "minekartTexture.c"
#include "carTexture.c"
#include "car2Texture.c"
#include "greenCheckPoint.c"
#include "greyCheckPoint.c"


#define pi 3.14159265

using namespace audiere;
using namespace std;

string arquivo;

bool keystates[256];

GLuint programID;
GLuint matrixID;        // Get a handle for our "MVP" uniform

//plane
GLuint vertexID1;
GLuint vertexBuffer1;
GLuint uvBuffer1;
GLuint textureID1;                  // Get a handle for our "myTextureSampler" uniform
std::vector<glm::vec3> vertices1;
std::vector<glm::vec2> uvs1;
std::vector<glm::vec3> normals1;

//hands
GLuint vertexID2;
GLuint vertexBuffer2;
GLuint uvBuffer2;
GLuint textureID2;                  // Get a handle for our "myTextureSampler" uniform
std::vector<glm::vec3> vertices2;
std::vector<glm::vec2> uvs2;
std::vector<glm::vec3> normals2;

//track
GLuint vertexID3;
GLuint vertexBuffer3;
GLuint uvBuffer3;
GLuint textureID3;                  // Get a handle for our "myTextureSampler" uniform
std::vector<glm::vec3> vertices3;
std::vector<glm::vec2> uvs3;
std::vector<glm::vec3> normals3;

#include "IA.c"

glm::vec3 inim[10000];
glm::vec3 inimpos;
float anglinim = 180;


//brick texture
GLuint textureID4;

//sand texture
GLuint textureID5;

//green check point texture
GLuint textureID6;

//grey check point texture
GLuint textureID7;

//utilizado pra definir qual textura dos checkpoints durante corrida
GLuint textureID8;

//opponent car texture
GLuint textureID9;

//player position
int currentCheckPoint = 1; //o checkpoint atual
int cameraLimit = 0; //utilizado pra fazer o efeito de ver o carro de lado quando faz curvas
int angleIncrement = 0; //faz o carro virar mais quando aperta espaço (cavalo de pau)
int nVoltas = 3; //numero de voltas pra completar a corrida
int tempoMinFinal, tempoMinInicial, tempoSegFinal, tempoSegInicial, tempoMinRecord, tempoSegRecord, contloop = 0;
float MAX_SPEED_FOWARD = 0.6; //velocidade maxima pra frente
float MAX_SPEED_BACKWARDS = 0.3; //velocidade maxima pra trás
float posx = -85; //coordenada x
float posy = 5; //coordenada y
float posz = -102.0; //coordenada z
float posxant = -85; //coordenada x
float poszant = -102.0; //coordenada z
float angl = 180; //utilizado pra virar carro, virar câmera, etc
float angleOffset = -90; //utilizado pra auterar o angl
float aceleracao = 0;
float speed = 0.02;
float nitro = 500;
bool cameraOn = true; //indica qual câmera está ativa; true = normal, false = aérea
bool playEndSound = true;
int finish = 0; //indica que a corrida acabou
glm::vec3 cameraPosition;
glm::vec3 cameraTarget;

struct Light {
    glm::vec3 position;
    glm::vec3 intensities; //a.k.a. the color of the light
};
Light gLight;
time_t tempoInicio, tempoFim;
struct tm *tempoCorrida;
AudioDevicePtr device(OpenDevice());
bool bateu = 0;
bool pause = 0;

//verifica se o carro está dentro ou fora da pista. Se estiver fora, sua velocidade é reduzida.
bool estaNaPista() {
    if  (posz <= -140 || posz >= 95.5 || posx <= -99 || posx >= 101 ||
        (posz >= 57 && posz <= 76 && posx <= 34 && posx >= -34) ||
        (posz >= 76 && posx >= 53) || (posz >= 76 && posx <= -54) ||
        (posz >= 45 && posz <= 56.5 && posx <= -54 && posx >= -79) ||
        (posz >= -120 && posz <= 45 && posx <= -49 && posx >= -79) ||
        (posz >= -1.5 && posz <= 37 && posx <= 81 && posx >= -49) ||
        (posz >= -120 && posz <= -67.5 && posx <= 27 && posx >= -49) ||
        (posz >= 37 && posz <= 56.5 && posx <= 81 && posx >= 54) ||
        (posz <= -21 && posx >= 47.5) ||
        (posz >= -48 && posz <= -21 && posx >= -29.5))
        {
        speed = 0.005;
        MAX_SPEED_FOWARD = 0.3;
        return false;
    }
    else {
        speed = 0.02;
        MAX_SPEED_FOWARD = 0.6;
        return true;
    }
}

void verifyCheckPoint() {
 //Verifica se o carro passou pelos checkpoints para saber quando completa uma volta
    switch (currentCheckPoint) {
        case 0:
            if (posx < -76 && posx > -106 && posz < -104 && posz > -108) {
                nVoltas--;
                if (nVoltas == 0) { //Acabou a corrida, mostra se venceu ou perdeu
                   if(finish != 2)
                    finish = 1;
                    time(&tempoFim);
                    tempoCorrida = localtime(&tempoFim);
                    tempoMinFinal = tempoCorrida->tm_min;
                    tempoSegFinal = tempoCorrida->tm_sec;
                    tempoMinFinal = tempoMinFinal - tempoMinInicial;
                    tempoSegFinal = tempoSegFinal - tempoSegInicial;
                    if (tempoSegFinal < 0) {
                        tempoMinFinal--;
                        tempoSegFinal = 60 + tempoSegFinal;
                    }
                }
                else
                    currentCheckPoint = 1;
            }
            break;
        case 1:
            if (posx < -65 && posx > -70 && posz < -111 && posz > -147)
                currentCheckPoint = 2;
            break;
        case 2:
            if (posx < 55 && posx > 20 && posz < -110 && posz > -115)
                currentCheckPoint = 3;
            break;
        case 3:
            if (posx < 20 && posx > 15 && posz < -40 && posz > -75)
                currentCheckPoint = 4;
            break;
        case 4:
            if (posx < -20 && posx > -55 && posz < -30 && posz > -35)
                currentCheckPoint = 5;
            break;
        case 5:
            if (posx < 107 && posx > 75 && posz < 10 && posz > 5)
                currentCheckPoint = 6;
            break;
        case 6:
            if (posx < 70 && posx > 65 && posz < 85 && posz > 50)
                currentCheckPoint = 7;
            break;
        case 7:
            if (posx < -75 && posx > -107 && posz < 50 && posz > 45)
                currentCheckPoint = 0;
            break;
    }

}


void loadTexture(unsigned int width, unsigned int height, const unsigned char * data) {

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, // target
        0, // level, 0 = base, no minimap,
        GL_RGB, // internalformat
        width, // width
        height, // height
        0, // border, always 0 in OpenGL ES
        GL_RGB, // format
        GL_UNSIGNED_BYTE, // type
        data // data
    );
}


int init_resources() {

    time(&tempoInicio);
    tempoCorrida = localtime(&tempoInicio);
    tempoMinInicial = tempoCorrida->tm_min;
    tempoSegInicial = tempoCorrida->tm_sec;

    int i;
    for (i = 0; i < 256; i++) {
        keystates[i] = false;
    }

    gLight.position = vec3(0.0f, 10.0f, -10.0f);
    gLight.intensities = vec3(1.0f, 1.0f, 1.0f);
	glClearColor(0.1f, 0.7f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

    //load objects
    bool res = loadOBJ("plane.obj", vertices1, uvs1, normals1);
    bool res2 = loadOBJ("car3.obj", vertices2, uvs2, normals2);
    bool res3 = loadOBJ("newTrack.obj", vertices3, uvs3, normals3);

    //setup vertexID
    glGenVertexArrays(1, &vertexID1);
    glBindVertexArray(vertexID1);

    glGenVertexArrays(1, &vertexID2);
    glBindVertexArray(vertexID2);

    glGenVertexArrays(1, &vertexID3);
    glBindVertexArray(vertexID3);


    //programID = LoadShaders( "vertexshader.vs", "fragmentshader.fs" );
    programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    matrixID = glGetUniformLocation(programID, "MVP");


    //loading textures

    //plane texture
    glGenTextures(1, &textureID1);
	//textureID1  = glGetUniformLocation(programID, "myTextureSampler");
    //generate and bind vertices and uvs
	glGenBuffers(1, &vertexBuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer1);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);
    glGenBuffers(1, &uvBuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer1);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);


    //skin texture
    //glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &textureID2);
   // loadTexture(terra.width, terra.height, terra.pixel_data);
	//textureID2  = glGetUniformLocation(programID, "myTextureSampler");
	glGenBuffers(1, &vertexBuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer2);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
    glGenBuffers(1, &uvBuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer2);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);


    //track texture
    glGenTextures(1, &textureID3);
	//textureID3  = glGetUniformLocation(programID, "myTextureSampler");
    //generate and bind vertices and uvs
	glGenBuffers(1, &vertexBuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer3);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);
    glGenBuffers(1, &uvBuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer3);
	glBufferData(GL_ARRAY_BUFFER, uvs3.size() * sizeof(glm::vec2), &uvs3[0], GL_STATIC_DRAW);


    //brick texture
    glGenTextures(1, &textureID4);
    //sand texture
    glGenTextures(1, &textureID5);
    //green check point texture
    glGenTextures(1, &textureID6);
    //grey check point texture
    glGenTextures(1, &textureID7);
    //opponent car texture
    glGenTextures(1, &textureID9);



    return programID;

}


void keyboardDown(unsigned char key, int x, int y) {

    keystates[key] = true;

    if (keystates['v']) { //troca entre visão terceira pessoa e visão aérea
        cameraOn = !cameraOn;
    }

}

void keyboardUp(unsigned char key, int x, int y) {

    keystates[key] = false;

}


void idle() {

if (pause == 0) {

    if (keystates['p']) {
        pause = 1;
    }

    if (finish != 1 &&finish != 2 )
        contloop++;

    inimpos = inim[contloop];
    anglinim = inimpos[1];
    inimpos[1] = 0.0f;

    if (inimpos[0] == 0.0f && inimpos[1] == 0.0f && inimpos[2] == 0.0f ) {
       finish = 2;                   //Inimigo ganhou!
       inimpos = inim[contloop-1];
       bateu = 0;

    } else if (sqrt((posx - inimpos[0])*(posx - inimpos[0]) + (posz - inimpos[2])*(posz - inimpos[2])) < 3.0f ) {//bateram?
        if(bateu == 0) {
            string arquivo2;
            if(rand() % 2 == 0){
                arquivo2 = "./crash4.mp3";
            }
            else {
                arquivo2 = "./crash3.mp3";
            }
            OutputStreamPtr sound(OpenSound(device, arquivo2.c_str(), false));
            sound->setRepeat(false);
            sound->setVolume(0.70f);
            sound->play();
        }
        if (inimpos[0] > posx && inimpos[2] > posz ) {//Inim frente
            posx = posxant;
            posz = poszant;

        }
        else if (inimpos[0] < posx && inimpos[2] < posz)//Inim atras
            contloop--;
        else
            contloop--;

        bateu = 1;

    }
    else
        bateu = 0;

    verifyCheckPoint();

    if (finish == 1 || finish == 2) {
            string arquivo2;
        if (finish == 1) {
            if (playEndSound) {
                arquivo2 = "./aplause.mp3";
                OutputStreamPtr sound(OpenSound(device, arquivo2.c_str(), false));
                sound->setRepeat(false);
                sound->setVolume(1.0f);
                sound->play();
                playEndSound = false;
            }
            printf("\n~~FINISH!~~\tVoce Ganhou a corrida! Tempo: %d:%d\n", tempoMinFinal, tempoSegFinal);
        }
        else {
            if (playEndSound) {
                arquivo2 = "./fail.mp3";
                OutputStreamPtr sound(OpenSound(device, arquivo2.c_str(), false));
                sound->setRepeat(false);
                sound->setVolume(1.0f);
                sound->play();
                playEndSound = false;
            }
            printf("\n~~FINISH!~~\tVoce Perdeu a corrida!");
        }

    }
    else {

        //verifica se o carro esta fora da pista (anda mais devagar se estiver)
        if (estaNaPista()) {
            //nitro
            if (keystates['0']) {
                if (nitro > 0) {
                    MAX_SPEED_FOWARD = 1.1;
                    MAX_SPEED_BACKWARDS = 0.6;
                    speed = 0.04;
                    nitro -= 2;
                }
                else
                    MAX_SPEED_BACKWARDS = 0.3;
            }
            else {
                MAX_SPEED_FOWARD = 0.6;
                MAX_SPEED_BACKWARDS = 0.3;
                speed = 0.02;
                if (nitro < 500)
                    nitro += 0.3;
            }
        }

        //Movimenta para frente e para trás
        if (keystates['s']) {   //marcha reversa
            if (aceleracao < MAX_SPEED_BACKWARDS)
                aceleracao = aceleracao + speed;
            else
                aceleracao = aceleracao - speed*3;
        }
        else
        if (keystates['w']) { //acelerador
            if (aceleracao > -MAX_SPEED_FOWARD)
                aceleracao = aceleracao - speed/2;
            else
                aceleracao = aceleracao + speed*3;
        }
        else {
            if (aceleracao > 0) //desaceleração natural (não está freiando nem acelerando)
                aceleracao = aceleracao - speed/2;
            if (aceleracao < 0)
                aceleracao = aceleracao + speed/2;
            if (aceleracao <= speed/2 && aceleracao >= -speed/2)
                aceleracao = 0;
        }

        //freio de mão
        if (keystates[' ']) {
            if (aceleracao > 0)
                aceleracao = aceleracao - speed*2;
            if (aceleracao < 0)
                aceleracao = aceleracao + speed*2;
            if (aceleracao <= speed*20 && aceleracao >= -speed*2)
                aceleracao = 0;
            angleIncrement = 1;
        }
        else
            angleIncrement = 0;

        //Gira pra direita e pra esquerda, não pode girar enquanto está parado
        if (keystates['d'])
            if (aceleracao != 0)
                if (aceleracao > 0) {
                    angl += 1.0f - angleIncrement;
                    if (cameraLimit > -16) {
                        angleOffset--;
                        cameraLimit--;
                    }
                }
                else {
                    angl -= 1.0f + angleIncrement;
                    if (cameraLimit < 16) {
                        angleOffset++;
                        cameraLimit++;
                    }
                }
        if (keystates['a'])
            if (aceleracao != 0)
                if (aceleracao < 0) {
                    angl += 1.0f + angleIncrement;
                    if (cameraLimit > -16) {
                        angleOffset--;
                        cameraLimit--;
                    }
                }
                else {
                    angl -= 1.0f - angleIncrement;
                    if (cameraLimit < 16) {
                        angleOffset++;
                        cameraLimit++;
                    }
                }
        if (!keystates['a'] && !keystates['d'] && cameraLimit != 0) {
            if (cameraLimit > 0) {
                angleOffset--;
                cameraLimit--;
            }
            if (cameraLimit < 0) {
                angleOffset++;
                cameraLimit++;
            }
        }

        //Controle para não sair das paredes de tijolo
        if (posz < -147) {
            posz = -146;
            aceleracao = 0;
        }
        if (posz > 107) {
            posz = 106;
            aceleracao = 0;
        }
        if (posx < -107) {
            posx = -106;
            aceleracao = 0;
        }
        if (posx > 107) {
            posx = 106;
            aceleracao = 0;
        }

        if (angl == 360)
            angl = 0;
        if (angl == -10)
            angl = 350;

        //Gira a câmera
        if (keystates['z'])
            angleOffset += 1;
            if (angleOffset == 361)
                angleOffset = 1;
        if (keystates['x'])
            angleOffset -= 1;
            if (angleOffset == 0)
                angleOffset = 360;

        posxant = posx;
        poszant = posz;
        posz -= aceleracao * cos(pi*angl/180);   //cos() e sin() usam radianos, então deve-se multiplicar o
        posx -= aceleracao * sin(pi*angl/180);   //angulo por pi e dividir por 180 para ter o valor certo

        printf("\nSpeed = %.1f\tNitro: %.0f\tLaps: %d", -100*aceleracao, nitro/10, nVoltas);
        if (nVoltas == 1)
            printf("\tFINAL LAP!");
    }
}
else {
    if (keystates['p']){
        pause = 0;
    }
}
}

//draw method
void drawMesh(int vAttri, GLuint vBuffer,
              int tAttri, GLuint tBuffer,
              GLuint texture, GLfloat uniform, int vSize) {

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vAttri);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    //Carregamento de texturas
    if (texture == textureID1) { //Textura da linha de chegada
        glBindTexture(GL_TEXTURE_2D, textureID1);
        loadTexture(planeTexture.width, planeTexture.height, planeTexture.pixel_data);
    }
    if (texture == textureID2) { //Textura do carro
        glBindTexture(GL_TEXTURE_2D, textureID2);
        loadTexture(car2Texture.width, car2Texture.height, car2Texture.pixel_data);
    }
    if (texture == textureID3) { //Textura da pista
        glBindTexture(GL_TEXTURE_2D, textureID3);
        loadTexture(trackTexture.width, trackTexture.height, trackTexture.pixel_data);
    }
    if (texture == textureID4) { //Textura das paredes
        glBindTexture(GL_TEXTURE_2D, textureID4);
        loadTexture(brickTexture.width, brickTexture.height, brickTexture.pixel_data);
    }
    if (texture == textureID5) { //Textura do chão
        glBindTexture(GL_TEXTURE_2D, textureID5);
        loadTexture(sandTexture.width, sandTexture.height, sandTexture.pixel_data);
    }
    if (texture == textureID6) { //Textura do chão
        glBindTexture(GL_TEXTURE_2D, textureID6);
        loadTexture(greenCheckPoint.width, greenCheckPoint.height, greenCheckPoint.pixel_data);
    }
    if (texture == textureID7) { //Textura do chão
        glBindTexture(GL_TEXTURE_2D, textureID7);
        loadTexture(greyCheckPoint.width, greyCheckPoint.height, greyCheckPoint.pixel_data);
    }
    if (texture == textureID9) { //Textura do chão
        glBindTexture(GL_TEXTURE_2D, textureID7);
        loadTexture(carTexture.width, carTexture.height, carTexture.pixel_data);
    }

    glUniform1i(texture, uniform);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(tAttri);
    glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
    glVertexAttribPointer(
        1,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

     glDrawArrays(GL_TRIANGLES, 0, vSize );
 }


void onDisplay() {

    if (cameraOn == true) { //Camera atras do carro
        cameraPosition  = glm::vec3(posx-10*cos(pi*(-angleOffset-angl)/180), posy, posz-10*sin(pi*(-angleOffset-angl)/180));//2
        cameraTarget    = glm::vec3(posx+10*cos(pi*(-angleOffset-angl)/180), posy*0.2+1, posz+10*sin(pi*(-angleOffset-angl)/180));//2
    }
    else {  //camera aérea
        cameraPosition  = glm::vec3(posx-1*cos(pi*(-angleOffset-angl)/180), posy+50, posz-1*sin(pi*(-angleOffset-angl)/180));//2
        cameraTarget    = glm::vec3(posx+1*cos(pi*(-angleOffset-angl)/180), posy*0.2+1, posz+1*sin(pi*(-angleOffset-angl)/180));//2
    }

   	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUseProgram(programID);

    glm::mat4 Projection = glm::perspective(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    glm::mat4 View       = glm::lookAt(
								cameraPosition, //glm::vec3(-20, 25, -10),
								cameraTarget,   //glm::vec3(-10, 10, -1),

								glm::vec3(0,1,0)
						   );

    glm::mat4 Model      = glm::mat4(1.0f);

    glm::mat4 MVP;

    //gLight.position=cameraPosition;
    glUniform3fv(glGetUniformLocation(programID, "light_position"), 1, glm::value_ptr(cameraTarget));
    glUniform3fv(glGetUniformLocation(programID, "light_intensities"), 1,glm::value_ptr(gLight.intensities));


    //pista
    MVP        = Projection * View * Model;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer3, 1, uvBuffer3, textureID3, 0, vertices3.size());

    //Start
    glm::mat4 startTrans = translate(mat4(1.0f), vec3(-90, 0.01, -106));
    glm::mat4 startScale = scale(mat4(1.0f), vec3(1.0f, 0.0f, 0.3f));
	MVP        = Projection * View * Model * startTrans * startScale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID1, 0, vertices1.size());

    //Sand
    glm::mat4 sandTrans = translate(mat4(1.0f), vec3(0, -0.01, 0));
    glm::mat4 sandScale = scale(mat4(1.0f), vec3(11, 0, 15));
	MVP        = Projection * View * Model * sandTrans * sandScale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID5, 0, vertices1.size());

    //parede 1
    glm::mat4 brickTrans1 = translate(mat4(1.0f), vec3(0, -5, 109)) * rotate(mat4(1.0f), 90.0f, vec3(1,0,0));
    glm::mat4 brickScale1 = scale(mat4(1.0f), vec3(11, 1, 1));
    MVP        = Projection * View * Model * brickTrans1 * brickScale1;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID4, 2, vertices1.size());

    //parede 2
    glm::mat4 brickTrans2 = translate(mat4(1.0f), vec3(0, -5, -149)) * rotate(mat4(1.0f), 90.0f, vec3(1,0,0));
    glm::mat4 brickScale2 = scale(mat4(1.0f), vec3(11, 1, 1));
    MVP        = Projection * View * Model * brickTrans2 * brickScale2;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID4, 2, vertices1.size());

    //parede 3
    glm::mat4 brickTrans3 = translate(mat4(1.0f), vec3(109, -5, 0)) * rotate(mat4(1.0f), 90.0f, vec3(0,0,1));
    glm::mat4 brickScale3 = scale(mat4(1.0f), vec3(1, 1, 15));
    MVP        = Projection * View * Model * brickTrans3 * brickScale3;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID4, 2, vertices1.size());

    //parede 4
    glm::mat4 brickTrans4 = translate(mat4(1.0f), vec3(-109, -5, 0)) * rotate(mat4(1.0f), 90.0f, vec3(0,0,1));
    glm::mat4 brickScale4 = scale(mat4(1.0f), vec3(1, 1, 15));
    MVP        = Projection * View * Model * brickTrans4 * brickScale4;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID4, 2, vertices1.size());

    //check1
    if (currentCheckPoint != 1)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check1Trans = translate(mat4(1.0f), vec3(-65, 0.01, -130));
    glm::mat4 check1Scale = scale(mat4(1.0f), vec3(0.3f, 0.0f, 0.7f));
	MVP        = Projection * View * Model * check1Trans * check1Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check2
    if (currentCheckPoint != 2)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check2Trans = translate(mat4(1.0f), vec3(37, 0.01, -115));
    glm::mat4 check2Scale = scale(mat4(1.0f), vec3(0.7f, 0.0f, 0.3f));
	MVP        = Projection * View * Model * check2Trans * check2Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check3
    if (currentCheckPoint != 3)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check3Trans = translate(mat4(1.0f), vec3(20, 0.01, -57));
    glm::mat4 check3Scale = scale(mat4(1.0f), vec3(0.3f, 0.0f, 0.7f));
	MVP        = Projection * View * Model * check3Trans * check3Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check4
    if (currentCheckPoint != 4)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check4Trans = translate(mat4(1.0f), vec3(-40, 0.01, -35));
    glm::mat4 check4Scale = scale(mat4(1.0f), vec3(0.7f, 0.0f, 0.3f));
	MVP        = Projection * View * Model * check4Trans * check4Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check5
    if (currentCheckPoint != 5)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check5Trans = translate(mat4(1.0f), vec3(90, 0.01, 5));
    glm::mat4 check5Scale = scale(mat4(1.0f), vec3(0.7f, 0.0f, 0.3f));
	MVP        = Projection * View * Model * check5Trans * check5Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check6
    if (currentCheckPoint != 6)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check6Trans = translate(mat4(1.0f), vec3(70, 0.01, 66));
    glm::mat4 check6Scale = scale(mat4(1.0f), vec3(0.3f, 0.0f, 0.7f));
	MVP        = Projection * View * Model * check6Trans * check6Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //check7
    if (currentCheckPoint != 7)
        textureID8 = textureID7;
    else
        textureID8 = textureID6;
    glm::mat4 check7Trans = translate(mat4(1.0f), vec3(-90, 0.01, 45));
    glm::mat4 check7Scale = scale(mat4(1.0f), vec3(0.7f, 0.0f, 0.3f));
	MVP        = Projection * View * Model * check7Trans * check7Scale;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer1, 1, uvBuffer1, textureID8, 0, vertices1.size());

    //carro
    glm::mat4 escCarro   = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f)); //0.2f
    glm::mat4 transCarro = glm::translate(mat4(1.0f), vec3(posx, 0.0f, posz));
    glm::mat4 rotCarro   = glm::rotate(mat4(1.0f), angl, vec3(0, 1.0f, 0));
    glm::mat4 rotFix     = glm::rotate(mat4(1.0f), 180.0f, vec3(0, 1.0f, 0));
    MVP        = Projection * View * Model * transCarro * escCarro * rotCarro *rotFix;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer2, 1, uvBuffer2, textureID2, 1, vertices2.size());

    //opponent
    glm::mat4 rotCarro2   = glm::rotate(mat4(1.0f), anglinim, vec3(0, 1.0f, 0));
    glm::mat4 transCarro2 = glm::translate(mat4(1.0f), inimpos);
    MVP        = Projection * View * Model * transCarro2 * escCarro * rotCarro2 *rotFix;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    drawMesh(0, vertexBuffer2, 1, uvBuffer2, textureID9, 1, vertices2.size());


    //disable
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

	glutSwapBuffers();
    glutPostRedisplay();

}


//libera recursos
void free_resources() {

    glDeleteBuffers(1, &vertexBuffer1);
	glDeleteVertexArrays(1, &vertexID1);
    glDeleteProgram(programID);

}


int main(int argc, char* argv[]) {
    srand(time(NULL));

    //iniciando musica
    arquivo = "./topgear.mp3";
    AudioDevicePtr device(OpenDevice());
    OutputStreamPtr sound(OpenSound(device, arquivo.c_str(), false));
    sound->play();
    sound->setRepeat(true);
    sound->setVolume(0.6f);


    //tela inicial
    printf("\n~~Super Top Gear~~  (low budget edition!)\nAs informacoes da corrida aparecerao nessa tela.\n");
    printf("Instrucoes:\n 'W' - Acelerar\n 'S' - Marcha reversa\n");
    printf(" 'A' - Virar para esquerda\n 'D' - Virar para direita\n '0' - Nitro\n");
    printf(" ' ' (barra de espaco) - Freio de mao (para fazer as curvas mais rapidamente)\n");
    do {
        printf("Escolha o numero de voltas (1~3): ");
        scanf("%d", &nVoltas);
    } while (nVoltas <= 0 || nVoltas > 3);


       switch (nVoltas) {
        case 1:
           if(rand() % 2 == 0){
              std::copy(std::begin(inim_1l_A), std::end(inim_1l_A), std::begin(inim));
           }else{
              std::copy(std::begin(inim_1l_B), std::end(inim_1l_B), std::begin(inim));
           }
            break;
        case 2:
          if(rand() % 2 == 0){
              std::copy(std::begin(inim_2l_A), std::end(inim_2l_A), std::begin(inim));
           }else{
              std::copy(std::begin(inim_2l_B), std::end(inim_2l_B), std::begin(inim));
           }
            break;
        case 3:
           if(rand() % 2 == 0){
              std::copy(std::begin(inim_3l_A), std::end(inim_3l_A), std::begin(inim));
           }else{
              std::copy(std::begin(inim_3l_B), std::end(inim_3l_B), std::begin(inim));
           }
            break;
    }


    //inicia janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Super Top Gear");

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }

    if (init_resources() != 0) {

        glutDisplayFunc(onDisplay);

        glutIgnoreKeyRepeat(1);
        glutKeyboardFunc(keyboardDown);
        glutKeyboardUpFunc(keyboardUp);
        glutIdleFunc(idle);

        glutMainLoop();
    }

    free_resources();
    return 0;
}


