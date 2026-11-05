wir3d v2.1 - efecto vocal VST3 para Windows 10/11 (Vocal Bend + Auto-Tune + Smile + Harmo + Beat Repeat)

INTERFAZ "NAVI"
  Una computadora retro completa: monitor CRT de plastico gris calido con tornillos, rejilla,
  placa de marca, LED de encendido y cables; escritorio oscuro; teclado en la parte inferior.
  Dentro de la pantalla: barra de menu con reloj, ventanas de SO retro (barra de titulo con
  rayas), scanlines, grano, vineta de cristal, brillo y glitch ocasional.
    - LAYER 01  monitor de afinacion (5 s): cian = nota detectada, rojo = nota corregida,
                + medidores IN/OUT de 16 segmentos.
    - LAYER 02  ojo animado (se abre con la voz y SONRIE con SMILE), nota objetivo en grande,
                medidor de CENTS (entrada frente a la nota objetivo) y lampara LOCK.
    - LAYER 03  11 perillas y selectores.
    - TECLADO   las 12 teclas C..B son funcionales (notas permitidas / tonica). Los triangulos
                sobre las teclas marcan el tono detectado (cian) y el tono de salida MEDIDO (rojo).
    - Menu      selector de presets (8 de fabrica + los tuyos) y boton SAVE.

CONTROLES
  INPUT   -12..+12 dB   ganancia de entrada (antes del detector y de todo el efecto).
  BEND    -12..+12 st   transposicion continua y automatizable.
  TUNE    0-100 %       cantidad de correccion hacia la nota objetivo.
  SPEED   0-100 %       tiempo de retune de ~300 ms (natural) a ~4 ms (efecto duro).
  GLIDE   0-800 ms      transicion (portamento) entre notas objetivo, independiente de SPEED.
                        Con 0 las notas cambian al instante; con 100-200 ms los saltos suenan cantados.
  TOLER   0-100 ct      tolerancia: no se corrige la parte de la desviacion menor que este valor.
  HUMAN   0-50 ct       micro-variacion lenta de afinacion para que no suene mecanico.
  VIBE    0-100 %       vibrato real (modulacion de afinacion) + deriva lenta.
  HARMO   0-100 %       armonizador "dimension": quinta (+7 st) estereo, -8/+8 cents con LFO
                        de 0.42 Hz, y delay ping-pong de cinta (wow/flutter, sincronizado al
                        tempo del host: corchea con puntillo).
  SMILE   0-100 %       macro por etapas (0 = transparente): aire y presencia + excitador
                        armonico (0-50 %), halo de voces ascendentes +5a/+8a/+8a+5a (12-100 %)
                        y reverb (35-100 %).
  FORMANT (boton)       conserva los formantes al cambiar el tono (BEND y correcciones grandes
                        no suenan a ardilla). Analisis LPC de orden 32 con celosia; es transparente
                        con TUNE 0. Alternarlo en caliente produce un pequeno hueco de ~40 ms.
  FILTER  -100..+100    filtro maestro bipolar: izquierda paso bajo (20 kHz -> 180 Hz),
                        derecha paso alto (20 Hz -> 3.8 kHz). Centro = sin filtro.
  RPT MIX 0-100 %       beat repeat: la primera rebanada de cada grupo se graba en vivo y se
                        repite; sigue el tempo y la posicion del host.
  OUTPUT  -12..+12 dB   trim de salida antes del techo suave (transparente hasta -3 dB,
                        curva suave hacia 0 dBFS; nunca pasa de 0 dBFS).
  MIX     0-100 %       seco/humedo (el seco se compensa en latencia).
  NOTAS   las 12 teclas del teclado son las notas permitidas: clic = permitir/excluir una nota;
          MAYUS+clic (o clic derecho) = elegir la tonica. Cambiar KEY o SCALE vuelve a rellenar
          las notas; RESET NOTES lo hace a mano. Con todas las notas excluidas no se corrige.
  SCALE   Chromatic, Major, Minor, Major Pent, Minor Pent, Harm Minor, Blues, Dorian,
          Phrygian, Mixolydian.
  RANGE   Wide, Soprano, Mezzo, Contralto, Tenor, Baritone, Bass: limita la busqueda de tono
          al registro del cantante (menos errores de octava y menos CPU).
  DIV     1/4, 1/8, 1/16, 1/32, Glitch (25 ms) para el beat repeat.
  A4      referencia de afinacion 430-450 Hz (defecto 440).

PRESETS DE FABRICA
  00 INIT, 01 WIRED NATURAL, 02 SNAP, 03 LAYER 07 HALO, 04 DIMENSION, 05 GLITCH TERMINAL,
  06 ANALOG GHOST, 07 BENDER. SAVE guarda el estado actual en
  %APPDATA%\wir3d\Presets (archivos .wir3dpreset) y aparece en la lista como USER // ...

AJUSTES RAPIDOS
  Natural:      preset 01, o TUNE 40-60 / SPEED 35-55 / VIBE 15
  Efecto duro:  preset 02 (TUNE 100, SPEED 100) y SCALE de tu cancion
  Bender:       TUNE 0 y automatiza BEND en Ableton
  Voz abierta:  SMILE 30-50 (brillo sin tocar los graves); mas arriba aparece el halo

PRUEBAS
  Carpeta Tests/: el detector (error <= 0.3 cents en 180 casos aleatorios, < 0.03 cents con el criterio de
  QPitch), el shifter (0.08 Hz de error) y los formantes se pueden verificar sin JUCE.
  La salida del plugin es identica con bloques de 1, 127 o 512 muestras, y L/R son identicos con
  una voz mono duplicada. Ver NOTICE.txt para licencias.

LATENCIA  ~37 ms (se informa al host; Ableton la compensa con PDC).

COMPILAR (lo mas facil)  Sigue LEEME_GITHUB.txt: GitHub compila por ti y deja el zip en Releases.
  Local (Visual Studio 2022 + CMake 3.22):
     cmake -B build -S . -G "Visual Studio 17 2022" -A x64
     cmake --build build --config Release
  Resultado: build/WIR3D_artefacts/Release/VST3/wir3d.vst3
  Copiar a C:\Program Files\Common Files\VST3 (borrando antes cualquier otro wir3d.vst3) y hacer Rescan.

LIMITACIONES
  - Monofonico: pensado para una voz limpia y sola; con coros o ruido habra artefactos.
  - Los formantes solo se preservan en la voz principal (no en HARMO ni en el halo de SMILE).
  - El beat repeat no incluye el filtro "ciber" resonante de wir3d.
  - Motor propio; nombres como Vocal Bender, Auto-Tune o Endless Smile pertenecen a sus
    autores y wir3d solo imita el tipo de efecto, sin copiar su codigo.
  - Todo el audio se probo numericamente con un arnes (JUCE simulado). La interfaz solo se
    comprobo en sintaxis y nombres: su aspecto real debe verificarse en la primera build.
