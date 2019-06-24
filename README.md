# Shader-Programmierung

## Implementierung

### Marching Cubes

Mit dem Marching-Cubes-Algorithmus ist es möglich dynamisch Geometrie zu erzeugen. In diesem Fall geschieht das mittels einer 3D-Textur die lediglich einen Farbkanal besitzt. Diese eine Farbe gibt den Dichtewert an. Die Dichtwerte müssen jedoch zunächst generiert werden, hierzu wird ein separater Framebuffer angelegt der die Werte
"scheibchenweise" in die 3D-Textur rendert. Die generierten Dichtewerte werden dann verwendet um den eigentlichen Algorithmus anzuwenden der basierend auf 8 Voxel
(dreidimensionale Texel) den entsprechenden Case feststellt und so die Vertices zurückgibt, aus denen Dreiecke generiert werden können. Ist der Vorgang abgeschlossen
erhält man im Optimalfall eine geschlossene Geometrie.

### Displacement Mapping

Displacement Mapping versucht durch Texturen Höhe bzw. Tiefe vorzutäuschen. Zum Einsatz kommen hier eine Heihtmap und Normalmap. Je nachdem mit welchem Winkel ein Objekt betrachtet wird werden die Texturkoordinaten verschoben. So ergibt der Eindruck von Tiefe.

### Soft Shadows

Die Schatten in diesem Programm werden mittels Variance Shadow Mapping (VSM) erstellt. Zunächst wird Shadow Mapping angewendet, die daraus resultierende Shadow Map wird allerdings mit Gaussian Blurring überarbeitet, sodass die Kantenübergänge weicher werden, was auch weichere Schatten zur Folge hat. Weiterhin wird dann die Wahrscheinlichkeit, dass ein bestimmter Punkt beleuchtet wird miteingerechnet, was die Übergänge von Licht und Schatten wesentlich sanfter erscheinen lässt, das die Wahrscheinlichkeit für Lichteinfall nach außen hin des Schattens der Shadowmap abfällt.

### Partikel

Das Partikelsystem wird in diesem Programm mithilfe eines Compute Shaders erstellt. So ist es möglich das Partikelsystem vollständig auf der GPU zu berechnen. Es werden alle notwendigen Attribute (Lebenszeit, Farbe, Größe etc.) auf die Grafikkarte geladen, die von nun an auf diese Buffer zugreift. Besonderes Augenmerk liegt hierbei auf dem Compute Shader der für jedes Partikel die entsprechenden Berechnungen druchführt und die neuen Werte für die Partikel zurück in den Buffer schreibt. Das ist effizienter als ein Transform Feedback Buffer, der seine Daten wieder zurück auf die CPU schreibt. 

### Tesselation

Eine Plane wird in beliebig einstellbar viele Dreiecke unterteilt und an eine Heightmap angepasst. Für Tesselation ist ein Tesselation Cotrol und ein Tesselation Evaluation Shader notwendig. Ersterer legt den Tesselierungsgrad fest und letzterer bestimmt die neuen Vertices.

## Tastenbelegung

WASD: Kamera bewegen
QE: Marching-Cubes-Säule hoch und runter bewegen

Z: Wireframe-Modus für die Tesselation-Plane aktivieren

U: Wireframe-Modus ausschalten

1: Tesselierung erhöhen

2: Tesselierung verringern

3: Säule rendern

4: Shadows rendern

5: Partikel rendern

6: Tesselation-Plane rendern