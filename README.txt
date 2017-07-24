Obsah:

3rdparty/ 		zdrojové súbory externých knižníc
data/ 			model sponza s textúrami
deployment/ 	predkompilované binárky pre debug aj release verziu
report/ 		text práce + zdrojáky
vfxDemo/ 		zdrojové súbory demo aplikácie
vfxEngine/ 		zdrojové súbory enginu
vfxFluid/ 		zdrojové súbory knižnice pre volumetrické efekty
video/			demonštračné video

Ako spustiť aplikáciu:
Podľa potreby debugovania stačí spustiť aplikáciu vfxDemo.exe v release alebo debug verzii.
Defaultne sa spustí demonštračná verzia s vykresľovaním efektu na pozadí. Pri spustení uvedenej
binárky s parametrom "sponza" sa spustí aplikácia s načítaním modelu scény sponza.

Ako preložiť aplikáciu:
1. vytvoriť adresár build v koreňovom adresári
2. cmd
3. cd build
4. cmake ..
5. preložiť projekt vfxDemo
6. install vo VS, prípadne ručne prekopírovať zložky data/, vfxFluid/resources do
	deployment/{BUILD_TYPE}