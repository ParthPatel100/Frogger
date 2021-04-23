/* GIMP RGB C-Source image dump (snake.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[50 * 33 * 2 + 1];
} snake = {
  50, 33, 2,
  "#\267\005\247J\237k\227H\237E\247d\267D\257\207\227\211\217I\237'\247D\267"
  "#\267#\267$\267$\267$\267#\267#\267#\277#\277#\277$\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267D\267g\257\002M$M#U)\247\005\247f\257\001M#"
  "M#UJ\247&\257D\267\003\267$\267$\267#\267#\267#\267#\267#\277#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267%\247(\237\005M\350D\351"
  "L\346TK\247*\237\347T\012M\350<\005E)\237%\257\004\267\003\267#\277#\277#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "G\237B]\010M\374\317\374\327\373\327\006M\006U\373\337\375\327\374\307\011=DMI"
  "\237F\267\003\267#\277#\277#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267I\227\003M\311D\300\000`\000\375\347\012E\351"
  "<\200\000\200\000\375\327\012Eh\065D=H\237E\257#\277\002\277#\277$\267$\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267i\237\002"
  "M\010M\340\000\200\000\376\337\013\065+\065\300\000\240\000\374\317\351\064j-\207\065B"
  "MG\247$\267#\277#\267$\267$\257D\257#\267#\267#\267#\267#\267#\267#\267#"
  "\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267$\247h\237$E(\065\013=+\065j-i%*\065k=)-I-i-h\065"
  "$\065\213\227F\247$\267$\267$\267$\267C\257#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267E\257G\237\214\207g-i%\212%i%\251%\211"
  "%i\035\211%\252%i-H%\210\065D=H\237%\257$\267$\267$\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267'\257i\237E\065\210-i%\211"
  "%\211%\251%\211%\211%\211\035\211%\212%\211%\211%f\065BMG\247E\267#\267\003\267"
  "#\267D\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267J\227"
  "#Eg\065\212-\211%\211%\252%\211\035\211%\211%\211-\212%i%\211%\252%G-#E(\237"
  "E\257#\267C\277#\267\003\267C\267#\267#\267#\267#\267#\267#\267#\267#\267#"
  "\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267f\065g\065h-I%i%\212%i\035\211%\251%\212%)-J\065j-\212-I\035\210"
  "-\205EI\227E\257\002\267#\277C\277#\267#\267#\267#\267#\267$\267#\277#\277"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267\211%H%j\065K\065*-J-J\065J\065*\065k=@\001*\065j-i%\212"
  "%h-D=H\227E\257C\277\002\267\003\267D\277#\267#\267#\267#\267$\267#\277#\277"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267\251%i-\352<\354L\354D\354D\313D\253L\254T\300\000"
  "\000\001*\065I-\211%\251\035\207-d=H\227%\257#\277#\267\003\267D\267#\267#\267#\267"
  "$\267$\267#\267#\277#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267f-H\065\000\001\300\000\340\000\000\001\300"
  "\000\340\000\240\000\300\000\013EJ\065\211-\211%\210\035\206-B=h\247$\267\342\266D\277"
  "$\267#\267#\267#\277#\267$\267$\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267\212"
  "\227$U\340\000\300\000\340\000\340\000\340\000\340\000\000\001\013=J-I%i\035\211%\251-f\065"
  "bM&\237E\267#\277#\267$\267#\267#\267#\277#\277$\267$\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267E\257(\247\347T\353L\353D\353D\353D\013E+\065J-\212-\211"
  "%\211%h-f-dEH\227F\257#\267#\277\003\267$\267#\267$\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267#\267#\267F\257+\237\007E)=J\065J-J-J%\212-i%"
  "h-g-e\065bMG\227E\247D\257\"\267#\267D\277\004\267\004\267$\267$\267#\267#\267"
  "\"\267C\267C\267\004\267#\267\"\267#\267#\267\003\267D\277#\267$\267$\267C\257"
  "C\267C\267\002\267$\277$\267\003\267C\267#\267#\267\"\267\004\257k\247%MH=j-j%"
  "\212%\251%\211%\211%g-\205=j\217G\237f\257E\257E\257E\247F\247\006\247'\257"
  "G\267\004\257$\257#\267\"\267C\277\002\267\002\267$\267$\267C\277#\267\003\267$\277"
  "#\267\003\267$\267$\267#\257C\267\"\267#\267\003\267$\277#\267#\267#\267#\267"
  "d\267&\257\353\236\300\022\340\002)\065\212-\211\035\211\035\251\035\211%G-bMh\237"
  "E\257%\257&\257(\237DEFEFM\300\012\010\247&\257D\267C\267\003\267#\277\003\277"
  "#\277D\267\002\257\002\257\003\267$\277\344\266#\267#\267#\267#\277\004\277$\277"
  "#\267D\277#\267#\267#\267#\267#\267#\267\005\257(\257\240\032\300\022\001\003)\065"
  "\252-\211%\251\035\251%F-dEH\237F\247%\257&\257\010\247$UG=)-)=\040\003\340\022"
  "(\247%\257\003\257$\277\003\277$\277#\277#\267C\267D\267\003\257F\267&\257%\267"
  "$\267#\267#\277\004\277\343\266$\277\003\257#\267#\267#\267D\277#\267#\267\011"
  "\257\340\"\200\022\240\012\001\003i-I%\212%\251%f-j\227&\247\005\257%\257F\257(\247"
  "\300\032\300\022\340\002)=J=\000\003\000\003\012\227h\247&\257$\277\343\266$\277#\267"
  "#\267C\267\002\267$\257G\237(\227I\257'\257C\257B\267\002\277\003\277#\267#\267"
  "#\267D\277#\267#\267#\267#\267+\247\240\032\240\032\341\012I\065i%i%\211-g%D"
  "\065h\247%\257\007\267)\257)\237\003U\300\012\341\002\341\002)=)=\000\003\000\003FM\002M(\247"
  "\006\267\004\277$\277#\267\004\267\004\257F\267G\247BM#E\340\022\340\032G\237E\247"
  "&\267\345\266%\267$\267$\267$\267$\267#\267#\267#\267\012\247\240\032\240\032"
  "\200\002i\065\211%\212%H%\215\177k\217I\237\011\247\300\032\240\022\300\002HEI\065"
  ")-!\003J=\350<\001\003\000\003H='E\340\012)\257\005\267$\277#\267\345\256H\257*\227DM"
  "&=\000\003\240\012\300\012F=eE\300\022+\267\011\257\007\257E\267\003\267#\267$\277#\267"
  "#\267\012\247\300\032\200\022\342\012i\065\211\035\212%\211-E=CE\003M\340\022\300"
  "\022\241\002\001\003I-j-\212-!\003)\065I=\040\003j\065J\065)=\340\002\340\022)\247'\247'\247"
  ")\257\300\022\040\003'=\011=\342\002\301\012\341\002J-I-\340\002\240\022\300\032\300\032"
  "\007\257%\267$\277\003\267#\267#\267\012\257\300\"\200\032\242\012*\065\212%i\035"
  "\211%h-G\065'=\000\003\340\002\011=I-\211%i%i-a\003H-\211-\211%i%i-)=!\003\000\003F=f=%="
  "\340\022\300\012\000\003I\065)\065\001\003\301\002*EJ%j-\001\003\240\002\300\002\000\023\240\022\011"
  "\247%\267#\267#\267#\267\011\257\240\032\240\"\302\022\012\065j-\211%\211%\211"
  "%i-J\065\001\003\001\003I=i%\211%\252-i%h-\211-\210%\210\035\251%\211-)\065\000\003i\065"
  "i-i%i\065\301\002\301\012)\065I-I\065\340\002!\003J\065I%j%*-J=)=\010=\340\002\340\022G"
  "\257#\257#\267C\267(\257\300\"\200\022\241\012\341\002I\065\211-\210%\251%\211"
  "%\211%\212-\211-i-h%\211%i%\211%\212-i%\211%\211\035\251%i%i\065A\003I%i%\212"
  "%J\065\240\002\301\012)\065I-I\065i\065H-h%\252-\211%\211-i-j-j-H-E=I\237E\257$"
  "\267D\277\006\257\011\257\240\032\300\022\340\002\010\065i\065\211-\211%\211\035\211"
  "\035\211\035\211%\211%\211%\211%i%\212%i%\211%\211%\252%i\035\252-H%\212-\212"
  "%i%\252%j-\001\003\341\002i\065i%H%\251\065\210%\210%h%\210%g-g-h-\210-\211%\210"
  "-CEH\227&\247E\257$\277\006\267\352\256\240\022\300\012)E\350\064i\065i%\212%\252"
  "%\211%i%i%\211\035\252\035\252%i%\251-h%i%\212%i\035\212\035\212%i%i%\212%\211"
  "\035j\035I-j-h%\251%\211%i%\212%\210%\207-f\065d=C=DEe=h%\211%h-\206=\213\227"
  "\040U\002\267%\267\007\267\240\032\300\022\301\002!\003)\065i-\212%i\035\211%\211-\211"
  "-\210-h%h%g-E\065e=e=f\065\210-\211%\212%\211%\212-i%\252%\212\035\212%\211"
  "%\250%g%\210-h-g-\207\065D=k\217i\227i\237I\237j\227\204E\206\065%-D=\"MH\227"
  "c\267#\257%\267I\267\200\022\201\012\240\002)Ei-\212%i%i%\211%h-e=dEd=cEh\237"
  "F\247&\247i\237d=g-\210%\211%i%\211%i\035\211\035\211%h%\207-e-EEDM\"MI\237"
  "H\247F\257e\247D\247E\257%\257F\247&\237h\247G\247F\257%\257C\257C\257#\267"
  "\007\257\200\032\241\022\301\022\300\002!\003j-i%\251-F-d=j\227I\237i\227h\237$\257"
  "C\277#\267%\257i\227d=g-\211%\211%\211%\211\035\250-E-d=C=k\217J\227(\237"
  "'\247%\257$\267#\267#\267C\267C\267#\267#\267#\277#\267\003\267#\267#\267#"
  "\267#\267#\267\007\257\240\032\201\022\241\022\301\012\341\002I\065i-f\065i\227G\247"
  "E\257%\257%\257%\257#\267#\267#\267$\267%\257i\227f\065\211%\211%\211%\211"
  "%g-i\227F\247F\247F\247%\257%\257$\267$\267$\267#\267#\267#\267#\267#\267"
  "#\267#\267#\267#\267#\267#\267",
};

