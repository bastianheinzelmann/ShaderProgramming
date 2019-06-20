#version 410 core

    layout(points) in;
    layout(points, max_vertices = 1) out;

    in float[] geoValue;
    out float outValue;

    void main()
    {
        for (int i = 0; i < 1; i++) {
			gl_Position = gl_in[i].gl_Position;
            outValue = geoValue[i];
            EmitVertex();
        }

        EndPrimitive();
    }
