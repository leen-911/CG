#include <iostream>

// 1. تضمين المكتبات
// ملاحظة: يجب تضمين GLEW قبل GLFW دائماً
#define GLEW_STATIC // لأننا نستخدم مكتبة glew32s.lib (Static)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float triOffsetX = 0.0f;
float triOffsetY = 0.0f;

// --- 2. كود المظللات (Shaders) ---

// A. كود Vertex Shader
// وظيفته: تحديد موقع رؤوس المثلث في الفضاء

//تعديل
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n" // نستقبل البيانات في الموقع 0
"layout (location = 1) in vec3 aColor;\n" //<--- [إضافة استقبال للون] 
"out vec3 ourColor;\n" // <--- [إضافة مخرج للون]
"uniform vec2 offset;\n"//
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x + offset.x,aPos.y + offset.y,aPos.z,1.0);\n" // نمرر الموقع كما هو
" ourColor = aColor;\n"
"}\0";

// B. كود Fragment Shader
// وظيفته: تحديد لون البكسلات )
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 FragColor;\n"
"uniform float uAlpha;\n" // <--- [إضافة] متغير للتحكم بالشفافية من الـ CPU
"void main()\n"
"{\n"
" FragColor = vec4(ourColor, uAlpha);\n" // استخدام Alpha في اللون النهائي
"}\n\0";

// دالة لمعالجة تغيير حجم النافذة من قبل المستخدم
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// دالة لمعالجة المدخلات (مثل الضغط على زر ESC)
void processInput(GLFWwindow* window)
{
    float speed = 0.001f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
 
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glPointSize(10.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        triOffsetX -= speed;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        triOffsetX += speed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        triOffsetY += speed;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        triOffsetY -= speed;
}

int main()
{
    // --- 3. تهيئة GLFW وإعداد النافذة ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // إصدار OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // النمط الحديث

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // خاص بأجهزة ماك
#endif

    // إنشاء كائن النافذة
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - First Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // جعل هذه النافذة هي سياق الرسم الحالي
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ربط دالة تغيير الحجم

    // --- 4. تهيئة GLEW ---
    glewExperimental = GL_TRUE; // تفعيل التقنيات الحديثة
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // --- 5. بناء وتجميع برنامج الشيدر (Shader Program) ---

    // ==========================================
    // [تعديل رقم 4]: تفعيل قدرات OpenGL المتقدمة
    // المكان: يجب أن تضاف هنا (بعد glewInit وقبل حلقة الرسم).
    // ==========================================
    glEnable(GL_DEPTH_TEST); // تفعيل اختبار العمق (لمنع تداخل الرسم)
    glEnable(GL_BLEND);      // تفعيل الدمج (للشفافية)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // معادلة الدمج

    // أ. تجميع Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // فحص الأخطاء
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ب. تجميع Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // فحص الأخطاء
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ج. ربط الشيدرز في برنامج واحد (Shader Program)
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    

    // فحص أخطاء الربط
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    //تعديل
    int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
    int alphaLocation = glGetUniformLocation(shaderProgram, "uAlpha");

    // حذف الشيدرز المنفصلة لأننا ربطناها في البرنامج ولم نعد بحاجة لها
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // --- 6. تعريف البيانات (المثلث) ---
    float tri_Vertices[] = {
        // position              // color (maroon)
         0.0f, -0.3f, 0.5f,      0.3f, 0.0f, 0.0f,
         0.6f, -0.3f, 0.5f,      0.3f, 0.0f, 0.0f,
         0.3f,  0.3f, 0.5f,      0.3f, 0.0f, 0.0f
    };
    

    float rect_Vertices[] = {
        // position              // color (green)
        -0.3f, -0.3f, 0.0f,     0.0f, 0.3f, 0.0f,
        0.3f, -0.3f, 0.0f,     0.0f, 0.3f, 0.0f,
        0.3f, 0.3f, 0.0f,      0.0f, 0.3f, 0.0f,

        -0.3f, -0.3f, 0.0f,      0.0f, 0.3f, 0.0f,
        0.3f,  0.3f, 0.0f,      0.0f, 0.3f, 0.0f,
        -0.3f,  0.3f, 0.0f,      0.0f, 0.3f, 0.0f
    };



    // --- 7. إعداد VBO و VAO ---
    unsigned int VBO1, VAO1;
    unsigned int VBO2, VAO2;

    glGenVertexArrays(1, &VAO1); // توليد المعرفات
    glGenBuffers(1, &VBO1);


    // خطوة مهمة جداً: اربط VAO أولاً قبل أي شيء آخر
    glBindVertexArray(VAO1);

    // اربط VBO وانسخ البيانات إليه
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_Vertices), tri_Vertices, GL_STATIC_DRAW);

    // شرح البيانات لـ OpenGL (Linking Vertex Attributes)
    // 0: الموقع في الشيدر
    // 3: حجم البيانات (3 floats لكل نقطة)
    // GL_FLOAT: نوع البيانات
    // GL_FALSE: لا نريد تطبيع البيانات
    // 6 * sizeof(float): الخطوة (Stride)
    // (void*)0: الإزاحة (بداية البيانات)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // تفعيل السمة رقم 0

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Rectangle 
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_Vertices), rect_Vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // فك الربط (اختياري ولكنه ممارسة جيدة)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 8. حلقة الرسم (Render Loop) ---

    while (!glfwWindowShouldClose(window))
    {
        // أ. معالجة المدخلات
        processInput(window);

        // ب. التنظيف (لون الخلفية)
        glClearColor(0.90f, 0.85f, 0.75f, 1.0f);  //the color beige
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//مسح مخزن العمق مع مخزن الألوان.
        // ج. الرسم
        //drawing the triangle

        glUseProgram(shaderProgram); // تفعيل الشيدر
        //تعديل
        glUniform2f(offsetLocation, triOffsetX, triOffsetY);
        glUniform1f(alphaLocation, 1.0f);   // fully visible


        glBindVertexArray(VAO1); // تفعيل كائن المثلث
        glDrawArrays(GL_TRIANGLES, 0, 3); // ارسم مثلثاً من 3 رؤوس
        // glBindVertexArray(0); // لا داعي لفك الربط في كل فريم

        //drawing the rectangular
        // 
        //تعديل
        glUniform2f(offsetLocation, 0, 0);
        glUniform1f(alphaLocation, 0.6f);   // Transparent

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // د. تبديل الـ Buffers ومعالجة الأحداث
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 9. التنظيف النهائي ---
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}