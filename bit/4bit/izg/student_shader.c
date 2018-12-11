/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{
#include<stdio.h>
void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);

  UniformLocation const viewMatrixLocation       = getUniformLocation(gpu, "viewMatrix");
  UniformLocation const projectionMatrixLocation = getUniformLocation(gpu, "projectionMatrix");

  Mat4 const*const view = shader_interpretUniformAsMat4(uniformsHandle, viewMatrixLocation);
  Mat4 const*const proj = shader_interpretUniformAsMat4(uniformsHandle, projectionMatrixLocation);

  Vec3 const*const position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  Vec3 const*const normal   = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

  Mat4 mvp;
  multiply_Mat4_Mat4(&mvp, proj, view);

  Vec4 pos4;
  copy_Vec3Float_To_Vec4(&pos4, position, 1.f);
  multiply_Mat4_Vec4(&output->gl_Position, &mvp, &pos4);
  
  Vec3 *const positionAttribute = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  Vec3 *const normalAttribute   = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);

  init_Vec3(positionAttribute, position->data[0], position->data[1], position->data[2]);
  init_Vec3(normalAttribute, normal->data[0], normal->data[1], normal->data[2]);
}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);
  UniformLocation const cameraPosition = getUniformLocation(gpu, "cameraPosition");
  UniformLocation const lightPosition  = getUniformLocation(gpu, "lightPosition");

  Vec3 const *position = fs_interpretInputAttributeAsVec3(gpu, input, 0); //  pozice ve world-space
  Vec3 const *normal   = fs_interpretInputAttributeAsVec3(gpu, input, 1); // normála ve world-space
  Vec3 const *cPos = shader_interpretUniformAsVec3(uniformsHandle, cameraPosition); // pozice kamery
  Vec3 const *lPos = shader_interpretUniformAsVec3(uniformsHandle, lightPosition);  // pozice světla
  
  // -------------------------------------------
  //      Výpočet difúzní intenzity světla
  // -------------------------------------------
  // Id = Il * rd * cosinus
  // Il = barva světla
  // rd = difuzní barva materiálu
  // cos = N * L
  // -------------------------------------------
  Vec3 difuze; // difúzní intenzita světla
  init_Vec3(&difuze, 0.f, 0.f, 0.f);
  //Vec3 lightColor;  // barva světla
  //Vec3 difuzeColor; // difuzní barva materiálu
  //init_Vec3(&lightColor, 1.f, 1.f, 1.f);
  //init_Vec3(&difuzeColor, 0.f, 1.f, 0.f);
  Vec3 norm;     // N = normalizovaná normála povrchu
  Vec3 lightDir; // L = normalizovaný vektor ke světlu
  
  // rozdíl mezi pozicí světla a pozicí ve world-space
  sub_Vec3(&lightDir, lPos, position);
  
  // normalizace
  normalize_Vec3(&norm, normal); 
  normalize_Vec3(&lightDir, &lightDir); 
  
  // výpočet cosinu
  float cosinus = dot_Vec3(&norm, &lightDir); // cos = N * L
  if(cosinus > 0)
  {
	// výpočet výsledné difúzní intenzity světla
    //difuze.data[0] = lightColor.data[0] * difuzeColor.data[0] * cosinus;    // optimalizace - násobení 0
    difuze.data[1] = /*lightColor.data[1] * difuzeColor.data[1] * */cosinus;  // optimalizace - násobení 1
    //difuze.data[2] = lightColor.data[2] * difuzeColor.data[2] * cosinus;    // optimalizace - násobení 0
  }

  // -------------------------------------------
  //      Výpočet spekulární intenzity světla
  // -------------------------------------------
  // Is = Il * rs * cos
  // Il = barva světla
  // rs = koeficient reflexe = spekulární barva materiálu
  // cos = (V * R)^ns
  // R = 2 * (N*L) * N - L
  
  Vec3 specular; // spekulární intenzita světla
  init_Vec3(&specular, 0.f, 0.f, 0.f);
  //Vec3 specularColor; // spekulární barva materiálu (Optimalizace: čistě bílá -> samé jedničky -> můžeme vynechat)
  //init_Vec3(&specularColor, 1.f, 1.f, 1.f);
  Vec3 camera; // normalizovaný vektor kamery
  float shininess = 40.f; //shininess
	
  // rozdíl mezi pozicí kamery a pozicí ve world-space
  sub_Vec3(&camera, cPos, position);
  // normalizace
  normalize_Vec3(&camera, &camera);

  Vec3 R;
  cosinus += cosinus; // cosinus = 2 * (N * L)
  multiply_Vec3_Float(&R, &norm, cosinus); // R = cosinus * N
  sub_Vec3(&R, &R, &lightDir); //R = R - L
  
  cosinus = dot_Vec3(&camera, &R); // cos = (V * R)^ns
  if(cosinus > 0)
  {
    // výpočet výsledné spekulární intenzity světla
    cosinus = powf(cosinus, shininess);
    init_Vec3(&specular, cosinus, cosinus, cosinus); // optimalizace - násobení jedničkou
    //specular.data[0] = lightColor.data[0] * specularColor.data[0];
    //specular.data[1] = lightColor.data[1] * specularColor.data[1];
    //specular.data[2] = lightColor.data[2] * specularColor.data[2];
    //multiply_Vec3_Float(&specular, &specular, cosinus);
  }
  
  // -------------------------------------------
  // Phong: Ip = Id + Is (ambientní nepoužíváme)
  // -------------------------------------------
  Vec3 phongColor; // výsledná barva
  add_Vec3(&phongColor, &difuze, &specular);

  // zapsání barvy
  copy_Vec3Float_To_Vec4(&output->color, &phongColor, 1.f);
}

/// @}
