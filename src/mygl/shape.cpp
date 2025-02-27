#include "shape.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Implementaci�n de la clase Cube.

// Constructor de Cube: inicializa los buffers y configura los atributos de v�rtice.
Cube::Cube()
{
    // Generaci�n y vinculaci�n de un Vertex Array Object (VAO) y un Vertex Buffer Object (VBO).
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Carga de los datos de los v�rtices en el buffer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Configuraci�n de los atributos de los v�rtices para posiciones.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configuraci�n de los atributos de los v�rtices para coordenadas de textura.
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configuraci�n de los atributos de los v�rtices para normales.
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}


// M�todo render: dibuja el cubo y aplica texturas si est�n disponibles.
void Cube::render(Shader shader, const ICamera& camera)
{
    shader.use(); // Activaci�n del shader.

    // Aplicaci�n de la textura difusa si est� disponible.
    if (diffuse_texture) {
        shader.set_int("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_texture);
    }

    // Aplicaci�n de la textura especular si est� disponible.
    if (specular_texture) {
        shader.set_int("material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_texture);
    }

    // Configuraci�n de las matrices de transformaci�n y la matriz normal para el shader.
    glm::mat4 model = transform.get_model_matrix();
    glm::mat4 projection = camera.get_projection_matrix();
    glm::mat4 view = camera.get_view_matrix();
    glm::mat3 normal = glm::transpose(glm::inverse(model));

    shader.set_mat4("model", model);
    shader.set_mat4("projection", projection);
    shader.set_mat4("view", view);
    shader.set_mat3("normal", normal);

    // Vinculaci�n del VAO y renderizado del cubo.
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// M�todo add_texture: carga una textura desde un archivo y la asocia a un ID de textura.
void Cube::add_texture(const char* file, unsigned int& texture)
{
    glGenTextures(1, &texture); // Generaci�n del ID de textura.

    int width, height, nrComponents;
    // Carga de la imagen y obtenci�n de sus dimensiones y componentes.
    unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        // Determinaci�n del formato de la imagen basado en el n�mero de componentes.
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // Vinculaci�n y configuraci�n de la textura.
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Configuraci�n de par�metros de la textura.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data); // Liberaci�n de los datos de la imagen.
    }
    else
    {
        std::cout << "Texture failed to load at path: " << file << std::endl;
        stbi_image_free(data); // Liberaci�n de los datos de la imagen en caso de fallo.
    }
}