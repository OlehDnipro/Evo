#pragma once

namespace NWater {


enum Root
{
	Samplers,
	Resources,
};

enum NResources
{
	WaterConst,
	ModelConstPBR,
	WaveTex,
	PlanarReflection,
	WaterNormalTile,
	EnvTex,

};

const char* RootItemNames[2][6] =
{

	{
		"ColorSampler"
	},

	{
		"WaterConst",
		"ModelConstPBR",
		"WaveTex",
		"PlanarReflection",
		"WaterNormalTile",
		"EnvTex"
	},

};

const char* RootItemTypes[2][6] =
{

	{
		"SamplerState"
	},

	{
		"SWaterConst",
		"SPBRModel",
		"Texture2D  ",
		"Texture2D  ",
		"Texture2D  ",
		"TextureCubeArray"
	},

};

static const char RootSig[] =
	"\2\0\0\0\10\0\0\0\1\0\0\0\7\0\0\0\6\0\0\0\6\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0"
	"\1\0\0\0\1\0\0\0\1\0\0\0DXBC&\17\370\232\351 `h\374\206L\251?\320\37\316\1\0\0\0\264\0\0\0\1\0\0\0$\0\0\0RTS0\210\0\0\0\2\0\0\0\2"
	"\0\0\0\30\0\0\0\0\0\0\0\210\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\60\0\0\0\0\0\0\0\0\0\0\0P\0\0\0\1\0\0\0\70\0\0\0\3\0\0\0\1\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0X\0\0\0\2\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\2\0\0\0";

static const char VS[] =
	"DXBC\205\303k\0\314\200\216\331\372\324O\314\303S\377h\1\0\0\0\212\25\0\0\6\0\0\0\70\0\0\0H\0\0\0\252\0\0\0\212\1\0\0\262\2\0\0V"
	"\15\0\0SFI0\10\0\0\0\0\0\0\0\0\0\0\0ISG1Z\0\0\0\2\0\0\0\10\0\0\0\0\0\0\0H\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\0\0\0\0\7\0\0\0\0\0\0\0\0"
	"\0\0\0Q\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\1\0\0\0\3\0\0\0\0\0\0\0Position\0TexCoord\0OSG1\330\0\0\0\5\0\0\0\10\0\0\0\0\0\0\0\250\0\0"
	"\0\0\0\0\0\1\0\0\0\3\0\0\0\0\0\0\0\17\360\0\0\0\0\0\0\0\0\0\0\264\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\1\0\0\0\3\374\0\0\0\0\0\0\0\0\0\0"
	"\275\0\0\0\1\0\0\0\0\0\0\0\3\0\0\0\1\0\0\0\14\363\0\0\0\0\0\0\0\0\0\0\306\0\0\0\2\0\0\0\0\0\0\0\3\0\0\0\2\0\0\0\3\374\0\0\0\0\0\0"
	"\0\0\0\0\317\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\3\0\0\0\7\370\0\0\0\0\0\0SV_Position\0TexCoord\0TexCoord\0TexCoord\0Position\0PSV0 \1"
	"\0\0$\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\1\0\0\0\2\5\0\2\4\0\0\0\1\0\0\0\20\0\0\0\2\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\70\0\0\0\0Position\0TexCoord\0TexCoord\0TexCoord\0TexCoord\0Position\0\0\3\0\0\0\0\0\0\0\1\0\0\0\2\0\0\0\20\0\0\0\1"
	"\0\0\0\0\0\0\0\1\0C\0\3\0\0\0\12\0\0\0\0\0\0\0\1\1B\0\3\0\0\0\0\0\0\0\0\0\0\0\1\0D\3\3\4\0\0\23\0\0\0\0\0\0\0\1\1B\0\3\2\0\0\34\0"
	"\0\0\1\0\0\0\1\1b\0\3\2\0\0%\0\0\0\2\0\0\0\1\2B\0\3\2\0\0.\0\0\0\0\0\0\0\1\3C\0\3\2\0\0\17\20\0\0\17 \0\0\17@\0\0\0\0\0\0P\1\0\0"
	"\240\2\0\0\0\0\0\0\0\0\0\0STAT\234\12\0\0`\0\1\0\247\2\0\0DXIL\0\1\0\0\20\0\0\0\204\12\0\0BC\300\336!\14\0\0\236\2\0\0\13\202 \0"
	"\2\0\0\0\23\0\0\0\7\201#\221A\310\4I\6\20\62\71\222\1\204\14%\5\10\31\36\4\213b\200\30E\2B\222\13B\304\20\62\24\70\10\30K\12\62b"
	"\210H\220\24 CF\210\245\0\31\62B\344H\16\220\21#\304PAQ\201\214\341\203\345\212\4\61F\6Q\30\0\0\10\0\0\0\33\214\340\377\377\377\377"
	"\7@\2\250\15\204\360\377\377\377\377\3 m0\206\377\377\377\377\37\0\11\250\0I\30\0\0\3\0\0\0\23\202`B L\10\6\0\0\0\0\211 \0\0\200"
	"\0\0\0\62\"\210\11 d\205\4\23#\244\204\4\23#\343\204\241\220\24\22L\214\214\13\204\304L\20\230\301\14\300\60\2\1$A0\214\60\0g\6\303"
	"\345\3\13b4\14\321L\376B\30\200\200\371\322\24Q\302\344\263\60\60\21\21\347\64\322\204\32a\0\6`\0\6`\0\6\340\300`\270|`A\214\206"
	"!\232\311_\10\3\20\60_\10``\"\"\316i\244\11\5\4\25#\0%8\10\231#\0\203\71\2\244\30\200\242(\213BL1\16EQ\26\200\234\243\206\313\237"
	"\260\207\220|n\243\212\225\230\374\342\266\21\1\0\0@\313=\303\345O\330CH~\10\64\303B\240 *\4\244H\12M\245\0\24\0\0\250\232#\10\212"
	"!)\213\242\\\204\15\4\234%,\200$\371\14\60E\310\345\27\213\3L>\356\343(\20N\222\246\210\22&\237\205\201\211\210\70\247\221&\324l"
	"\3\60\0\3\60\0\3\60\0\7ISD\11\223\317\246\333j:\3\261 b\13\6`\246\66\30\7v\10\207y\230\7\67\240\205r\300\7z\250\7y(\7\71 \5>\260"
	"\207r\30\7zx\7y\340\3s`\207w\10\7z`\3\60\240\3?\0\3?\320\3=h\207t\200\207y\370\5z\310\7x(\7\24(3\211\301\70\260C8\314\303<\270\1"
	"-\224\3>\320C=\310C9\310\1)\360\201=\224\303\70\320\303;\310\3\37\230\3;\274C8\320\3\33\200\1\35\370\1\30\370\1\22\2o\246\64\30\7"
	"v\10\207y\230\7\67\240\205r\300\7z\250\7y(\207Q\250\7q(\207P\220\7y\10\207|\340\3{(\207q\240\207w\220\7>0\7vx\207p\240\7\66\0\3:"
	"\360\3\60\360\3\24\4\303\10\2p\217\64E\224\60\371/\21M\304%6H\30\302s\244)\242\204\311O\221\213X\304\6\25A\30\206\303\61B\0\3\23"
	"\21qN#M(\340\22A\1\0\0\0\23\24r\300\207t`\207\66h\207yh\3r\300\207\15\257P\16m\320\16zP\16m\0\17z0\7r\240\7s \7m\220\16q\240\7s "
	"\7m\220\16x\240\7s \7m\220\16q`\7z0\7r\320\6\351\60\7r\240\7s \7m\220\16v@\7z`\7t\320\6\346\20\7v\240\7s \7m`\16s \7z0\7r\320\6\346"
	"`\7t\240\7v@\7m\340\16x\240\7q`\7z0\7r\240\7v@\7:\17d\220!#%@\0V\0\300\220\7\1\2\0\0\0\0\0\0\0\0\200!\17\3\4@\0\0\0\0\0\0\0\0C\36"
	"\7\10\0\1\0\0\0\0\0\0\0\206<\23\20\0\3\0\0\0\0\0\0\0\14y* \0\2\0\0\0\0\0\0\0\30\362`@\0\14\0\0\0\0\0\0\0\220\5\2\0#\0\0\0\62\36\230"
	"\30\31\21L\220\214\11&G\306\4CZ\12\241\10\312`\4\200\224\22\30\1(\206\2\26(\202B(\203r(\211\2\14(\314\200\2\15(\334\200\2\16(\354"
	"\200\2\17(\374\200\2\24(L\201\2\25(\\\201B)\220R(\230\302+\240\2+\17ZJ\202\214\31\0*f\0\210\230\1\240`\6\200\270\31\0\352f\0\310"
	"\233\1\240o\6\200\300\31\0\22g\0h\234\1 r\6\200\312\261\20\343yA\342\5\221\27\204\0X\1+\0\0y\30\0\0-\1\0\0\32\3L\220F\2\23D5\30c"
	"\13s;\3\261+\223\233K{s\3\231q\271\1A\241\13;\233{\221*b*\12\232*\372\232\271\201y1Ks\13cK\331\20\4\23\4e\231 (\314\6a 6\10\4A\301"
	"nn\202\240\64\33\206\3!&\10F\32\220\31\23\273\12\243+\223\33zs\233\243\233 (\316\6\204P\26\202\30\30`C\320l \0\300\1&\10\\\33ph\263"
	"\203\233 (\317\4A\201\66\14\323\64L\20\224h\202\240H\23\4e\332\200 \221DU\204u\321\30\13k\203z\233\233 (\324\6\3\311*\315\272\310"
	"\314\321\311\225\205\265=\231\231\315\225\321M\20\224j\203\201pUg]4\304\336\360\232\302\360&\10\212\265\301@\276\12\14\254\213\4"
	"\\\30\231\314\4A\271\66\30\210\30Tc`]4\304\336\360\232\322\334&\10\12\266\301@\312\240\62\3\353\"\1\27F63AP\262\15\6\202\6U\32X\27"
	"\225\261:\261\262\241\62\67\272\62\271\11\202\242m0\220\65\250\330\300\272\370\334\205\331\225=\231\231\315\225\321M\215\205\261"
	"\225M\20\224m\203\201\270A\365\6\326\305D\354\15\257)\14\317(Ln\202\240p\33\14$\16*9\260.\22pad5\23\4\245\333` tP\325\201u1\21{\303"
	"kJs3\12\223\233 (\336\6\3\271\203\12\17\254\213\4\\\30\231\315\4A\371\66\30\210\36T{`]\33\34\6\333\274\60 \203\63P\203\66\200\203"
	"\71\260\203<\340\203\11B\347\6,\332\336\310\312\330\66 \310\37HTEX\27\221\266\60\272\62\271\264\60\266\15\6\22\12\225f]L\304\302"
	"\346\312\206\336\330\336\344\66\30\310(T``]\34\340\302\310\66\30H)Tc`]\33\12\63\0\5Q \5S\230 |p0AP\300\200\1\332\6\3Q\205\212\260"
	".\22mips\23\4%\14\66\20\10+T\255\60A\10\330`\203@\274\302\206\0\26\66\10D,l Ra\25\\A\26&\10\336\33\320@\13s#c+\233 (b\260\301@j\241"
	"\",[\330 L\267\60A\0\203\70\330\60\264\302*\310\302\4A\14\344`\202\240\214\1\7\270\267\71\"Position\33\20\244\27*b\362\5\353\242"
	"PgG\204\252\14o\350\355M\216l\3\202\200C\325\12S8X\327\206\201\27~A\34&\10c0\7\23\4\205\14\270LY}A\275\315\245\321\245\275\271m@"
	"\220^\250\210\311\34\254\213\11U\31\336\320\333\233\34\31\314\6\4\1\207\252\25&t\260.\36waveUV1L\250\312\360\206\336\336\344\310"
	"b6 \210:T\274\60\255\203u\361\270\13\263+\253\262\222\231 (e\300\204\252\14o\350\355M\216Lf\3\202\264C\345\16\323;X\27\11\270\267"
	"\271\253\11\202b\6L\240\336\346\322\350\322\336\334`6 H<T\362\60\315\203um0\312\341\34\322\201\35\340\201\36&\10d@\7T\256\302\350"
	"\312\344\206\336\334\346\350\66\20\310=T\304\6\201\301\207\11\2\261\6\23\204B\15\66T\4\324\7~p\12\250\60\13\264\200\13\271\240\13"
	"\273\60\16\344P\17\366\220\17\372\320\7\373\220\17\23\204$\330\0l\30\10\177\360\207\15\301?l\30\206~\0\211\11B\31\324\301\206@$H"
	"\264\205\245\271M\20\26\64\230 ,\312\206\200\230 ,\311\6\241\262\66,\204/\224\204I\234\204I\14(A\230DJL\20\26d\203PU\33\226!\34J"
	"\302$N\302$\206\225\30L\202%6\10*\321\22\23\204\345\230 (g\260A\250`b\303B\230CI\240\304I\274\304\360\22\204I\304\304\206e\10\207"
	"\222\60\211\223X\211a%\6\223`\211\15\301\260a\221\302\241$L\202&VbX\211a%XbC mX\252p(\11\223\260\211\225\30VB2\11\226\330\260L\276"
	"P\22&q\22+1\240De\22)\261\241\220\211\231\250\211\233\300\211\15\203K\344\4\260\241\350\7\222\320\211\7\250\302\306f\327\346\222"
	"FV\346F7%\10\252\220\341\271\330\225\311\315\245\275\271M\11\210&dx.valverS\202\242\16\31\236\313\34Z\30Y\231\\\323\33Y\31\333\224"
	"\0)C\206\347\"W6\367V'7V67%p*\221\341\271\320\345\301\225\5\271\271\275\321\205\321\245\275\271\315M\21\366\1$\352\220\341\271\330"
	"\245\225\335%\221M\321\205\321\225M\11D\242\16\31\236K\231\33\235\\\36\324[\232\33\335\334\224@'\0\0y\30\0\0I\0\0\0\63\10\200\34"
	"\304\341\34f\24\1=\210C8\204\303\214B\200\7yx\7s\230q\14\346\0\17\355\20\16\364\200\16\63\14B\36\302\301\35\316\241\34f0\5=\210C"
	"8\204\203\33\314\3=\310C=\214\3=\314x\214tp\7{\10\7yH\207pp\7zp\3vx\207p \207\31\314\21\16\354\220\16\341\60\17n0\17\343\360\16\360"
	"P\16\63\20\304\35\336!\34\330!\35\302a\36f0\211;\274\203;\320C9\264\3<\274\203<\204\3;\314\360\24v`\7{h\7\67h\207rh\7\67\200\207"
	"p\220\207p`\7v(\7v\370\5vx\207w\200\207_\10\207q\30\207r\230\207y\230\201,\356\360\16\356\340\16\365\300\16\354\60\3b\310\241\34"
	"\344\241\34\314\241\34\344\241\34\334a\34\312!\34\304\201\35\312a\6\326\220C9\310C9\230C9\310C9\270\303\70\224C8\210\3;\224\303/"
	"\274\203<\374\202;\324\3;\260\303\214\310!\7|p\3r\20\207sp\3{\10\7y`\207p\310\207w\250\7z\0\0\0\0q \0\0\34\0\0\0\6\60\11\1\14LD\304"
	"\71\215\64\375\13a\0\2f\4\333p\371\316\343\13\1U\24DT:\300P\22\6 `~q\333f \15\227\357<\276\20\21\300D\204@3,\204\11L\303\345;\217"
	"\277\70\300 6\17\65\371\305m\333@5\\\276\363\370\322\344D\4JM\17\65\371\305m[\201\64\\\276\363\370\23\21M\10\20a~q\333\26@0\0\322"
	"\0\0\0\0\0\0DXIL,\10\0\0`\0\1\0\13\2\0\0DXIL\0\1\0\0\20\0\0\0\24\10\0\0BC\300\336!\14\0\0\2\2\0\0\13\202 \0\2\0\0\0\23\0\0\0\7\201"
	"#\221A\310\4I\6\20\62\71\222\1\204\14%\5\10\31\36\4\213b\200\24E\2B\222\13B\244\20\62\24\70\10\30K\12\62R\210H\220\24 CF\210\245"
	"\0\31\62B\344H\16\220\221\"\304PAQ\201\214\341\203\345\212\4)F\6Q\30\0\0\10\0\0\0\33\214\340\377\377\377\377\7@\2\250\15\204\360"
	"\377\377\377\377\3 m0\206\377\377\377\377\37\0\11\250\0I\30\0\0\3\0\0\0\23\202`B L\10\6\0\0\0\0\211 \0\0\61\0\0\0\62\"H\11 d\205"
	"\4\223\"\244\204\4\223\"\343\204\241\220\24\22L\212\214\13\204\244L\20p#\0%\0\24f\0\346\10\300`\216\0)\306 \204\24B\246\30\200\20"
	"R\6\241\243\206\313\237\260\207\220|n\243\212\225\230\374\342\266\21\61\306\30T\356\31.\177\302\36B\362C\240\31\26\2\5\253\20\212"
	"\60B\255\24\203\214\61\350\315\21\4\305`\244\20\22I\16\4\14#\20C\22\324a\204a83\30.\37X\20\243a\210f\362\27\302\0\4\314\227\246\210"
	"\22&\237\205\201\211\210\70\247\221&\324\330;\356\270\343\216;\356\70\60\30.\37X\20\243a\210f\362\27\302\0\4\314\27\2\30\230\210"
	"\210s\32iB\1\246\234\10\4\0\0\23\24r\300\207t`\207\66h\207yh\3r\300\207\15\257P\16m\320\16zP\16m\0\17z0\7r\240\7s \7m\220\16q\240"
	"\7s \7m\220\16x\240\7s \7m\220\16q`\7z0\7r\320\6\351\60\7r\240\7s \7m\220\16v@\7z`\7t\320\6\346\20\7v\240\7s \7m`\16s \7z0\7r\320"
	"\6\346`\7t\240\7v@\7m\340\16x\240\7q`\7z0\7r\240\7v@\7C\236\0\0\0\0\0\0\0\0\0\0\0\206<\6\20\0\1\0\0\0\0\0\0\0\14y\20 \0\4\0\0\0\0"
	"\0\0\0\30\362\64@\0\14\0\0\0\0\0\0\0\60\344y\200\0\10\0\0\0\0\0\0\0`\310#\1\1\60\0\0\0\0\0\0\0@\26\10\0\16\0\0\0\62\36\230\24\31"
	"\21L\220\214\11&G\306\4C\"%0\2P\10\305P\300\2EP\6TJb\4\240\14\212\240\20H\317\0\320\36\13\61\236\27$^\20yA\10\200\25\260\2y\30\0"
	"\0n\0\0\0\32\3L\220F\2\23D5\30c\13s;\3\261+\223\233K{s\3\231q\271\1A\241\13;\233{\221*b*\12\232*\372\232\271\201y1Ks\13cK\331\20"
	"\4\23\4\302\230 \20\307\6a &\10\4\262A\30\14\12vs\23\4\"\331\60 \11\61A\320$\2\23\4B\331\200\20\13C\20C\3l\10\234\15\4\0<\300\4A"
	"\0\66\0\33\6B\222\66\4\323\206a\210\250\11\302\66m\10,\22mainD\240\336\346\322\350\322\336\334&\10E3A(\234\15\1\61A(\236\15\13\221"
	"i\33\267\15\35\261\1D\250\312\360\206\336\336\344\310&\10\5\264a\31>m\343\266\1\14\206\15\330 xa\300e\312\352\13\352m.\215.\355\315"
	"m\202PD\33\26b\14\264\216#\203\201\14\210\15\330\260\14\237\266q`0\200\301\260\1\23\4b\331\20\14\33\226\63\370\264\15\15\300`\0\203"
	"\1\14\200\11\2\301l\10\316`\303\242\6\237\266\255\1\30\14`p\6\33\260a12m\343\300`\350\324`\3\66\24e`\6i\300\6m\260a\20\3\67\0\66"
	"\24\21\366\6\20P\205\215\315\256\315%\215\254\314\215nJ\20T!\303s\261+\223\233K{s\233\22\20M\310\360\\\354\302\330\354\312\344\246"
	"\4F\35\62<\227\71\264\60\262\62\271\246\67\262\62\266)AR\206\14\317E\256l\356\255Nn\254lnJ\360T\"\303s\241\313\203+\13rs{\243\13"
	"\243K{s\233\233\22Pu\310\360\\\354\322\312\356\222\310\246\350\302\350\312\246\4V\35\62<\227\62\67:\271<\250\267\64\67\272\271)\301"
	"\33\0\0\0\0y\30\0\0I\0\0\0\63\10\200\34\304\341\34f\24\1=\210C8\204\303\214B\200\7yx\7s\230q\14\346\0\17\355\20\16\364\200\16\63"
	"\14B\36\302\301\35\316\241\34f0\5=\210C8\204\203\33\314\3=\310C=\214\3=\314x\214tp\7{\10\7yH\207pp\7zp\3vx\207p \207\31\314\21\16"
	"\354\220\16\341\60\17n0\17\343\360\16\360P\16\63\20\304\35\336!\34\330!\35\302a\36f0\211;\274\203;\320C9\264\3<\274\203<\204\3;\314"
	"\360\24v`\7{h\7\67h\207rh\7\67\200\207p\220\207p`\7v(\7v\370\5vx\207w\200\207_\10\207q\30\207r\230\207y\230\201,\356\360\16\356\340"
	"\16\365\300\16\354\60\3b\310\241\34\344\241\34\314\241\34\344\241\34\334a\34\312!\34\304\201\35\312a\6\326\220C9\310C9\230C9\310"
	"C9\270\303\70\224C8\210\3;\224\303/\274\203<\374\202;\324\3;\260\303\214\310!\7|p\3r\20\207sp\3{\10\7y`\207p\310\207w\250\7z\0\0"
	"\0\0q \0\0\30\0\0\0\66\260\15\227\357<\276\20PEAD\245\3\14%a\0\2\346\27\267m\5\322p\371\316\343\13\21\1LD\10\64\303BX\300\64\\\276"
	"\363\370\213\3\14b\363P\223_\334\266\11T\303\345;\217/MND\240\324\364P\223_\334\266\21H\303\345;\217?\21\321\204\0\21\346\27\267"
	"m\0\4\3 \15\0\0a \0\0\214\0\0\0\23\4A,\20\0\0\0\10\0\0\0\24G\0\250\224\0\221R(\256\31\200\262+9\32c\4 \10\202*\30\214\21\200 \10"
	"\312`\0\0#\6\11\0\202` aCcY\311\210A\2\200 \30\30\235\205]\220\61b\220\0 \10\6\206we\230r\214\30$\0\10\202\201\361aY&!#\6\11\0\202"
	"``\200A\246iL2b\220\0 \10\6F\30h\333&)V\34\362\261\2\221\317\210A\2\200 \30 c\360h\336%\214\30$\0\10\202\1\62\6\217\346A\201\35\212"
	"|\354X\344\63b\220\0 \10\6H\31D\33\30d\302\210A\2\200 \30 e\20m` \5#\6\7\0\202`\320\220\201\243\200\301hB\0\214&\10\301h\302 \214"
	"&\20\303\210\301\1\200 \30\64i0=f0\232\20\0\243\11B0\232\60\10\243\11\304\60bp\0 \10\6\215\33`\224\31\214&\4\300h\202\20\214&\14"
	"\302h\2\61\214\30\34\0\10\202A3\7]\246\6\243\11\1\60\232 \4\243\11\203\60\232@\14\66m\362\31\61@\0\20\4\203\7\17\312`\332\202\21"
	"\3\4\0A0x\362\300\14\246-\260`\202\216I\237|F\14\20\0\4\301\340\341\203\64\220\276`\304\0\1@\20\14\236>P\3\351\13,\220\240c\321\30"
	"\310g\304\0\1@\20\14\36Ph\203h\14\202\21\3\4\0A0xB\301\15\242\61\10,\210\240c\320\31\310g\304\0\1@\20\14\36R\210\3\350\14\202\21"
	"\3\4\0A0xJA\16\240\63\10,\200\240\63b\220\0 \10\6H*\324\1)\220B\37\64#\6\11\0\202`\200\244B\35\220\2)\330A2b\220\0 \10\6H*\324\1"
	")\220\302\36\24#\6\11\0\202`\200\244B\35\220\2)\360A0b\220\0 \10\6H*\324\301(\220B\37\254\301\210A\2\200 \30 \251P\7\243@\12v\240"
	"\6#\6\11\0\202`\200\244B\35\214\2)\354A\32\214\30$\0\10\202\1\222\12uP\12\244\320\7m0b\220\0 \10\6H*\324A)\220\202\35\260\1\2\0\0"
	"\0\0";

static const char PS[] =
	"DXBC)\326\15l\350\377\276\66\20\251 S`)%\24\1\0\0\0\236$\0\0\6\0\0\0\70\0\0\0H\0\0\0(\1\0\0b\1\0\0\352\2\0\0\216\20\0\0SFI0\10\0"
	"\0\0\0\0\0\0\0\0\0\0ISG1\330\0\0\0\5\0\0\0\10\0\0\0\0\0\0\0\250\0\0\0\0\0\0\0\1\0\0\0\3\0\0\0\0\0\0\0\17\0\0\0\0\0\0\0\0\0\0\0\264"
	"\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\1\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\275\0\0\0\1\0\0\0\0\0\0\0\3\0\0\0\1\0\0\0\14\0\0\0\0\0\0\0\0\0\0\0"
	"\306\0\0\0\2\0\0\0\0\0\0\0\3\0\0\0\2\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\317\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\3\0\0\0\7\0\0\0\0\0\0\0SV_P"
	"osition\0TexCoord\0TexCoord\0TexCoord\0Position\0OSG12\0\0\0\1\0\0\0\10\0\0\0\0\0\0\0(\0\0\0\0\0\0\0@\0\0\0\3\0\0\0\0\0\0\0\17\360"
	"\0\0\0\0\0\0SV_Target\0PSV0\200\1\0\0$\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\0\0\0\0\5\1\0\4\1\0\0\0\7\0"
	"\0\0\20\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0\0\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\3\0\0\0\0\0\0\0\1\0\0\0\1\0\0\0\3\0\0\0\0\0\0\0\2\0\0\0\2\0\0\0\3\0\0\0\0\0\0\0\3\0\0\0\3\0\0\0(\0\0\0\0TexCoord\0T"
	"exCoord\0TexCoord\0Position\0\0\0\0\3\0\0\0\0\0\0\0\1\0\0\0\2\0\0\0\20\0\0\0\0\0\0\0\0\0\0\0\1\0D\3\3\4\0\0\1\0\0\0\0\0\0\0\1\1B"
	"\0\3\2\0\0\12\0\0\0\1\0\0\0\1\1b\0\3\2\0\0\23\0\0\0\2\0\0\0\1\2B\0\3\2\0\0\34\0\0\0\0\0\0\0\1\3C\0\3\2\0\0\0\0\0\0\0\0\0\0\1\0D\20"
	"\3\0\0\0\7\0\0\0\7\0\0\0\0\0\0\0\0\0\0\0\7\0\0\0\7\0\0\0\7\0\0\0\7\0\0\0\7\0\0\0\7\0\0\0\0\0\0\0\0\0\0\0\7\0\0\0\7\0\0\0\7\0\0\0"
	"\0\0\0\0STAT\234\15\0\0`\0\0\0g\3\0\0DXIL\0\1\0\0\20\0\0\0\204\15\0\0BC\300\336!\14\0\0^\3\0\0\13\202 \0\2\0\0\0\23\0\0\0\7\201#"
	"\221A\310\4I\6\20\62\71\222\1\204\14%\5\10\31\36\4\213b\200\30E\2B\222\13B\304\20\62\24\70\10\30K\12\62b\210H\220\24 CF\210\245\0"
	"\31\62B\344H\16\220\21#\304PAQ\201\214\341\203\345\212\4\61F\6Q\30\0\0\10\0\0\0\33\214\340\377\377\377\377\7@\2\250\15\204\360\377"
	"\377\377\377\3 m0\206\377\377\377\377\37\0\11\250\0I\30\0\0\3\0\0\0\23\202`B L\10\6\0\0\0\0\211 \0\0\247\0\0\0\62\"\210\11 d\205"
	"\4\23#\244\204\4\23#\343\204\241\220\24\22L\214\214\13\204\304L\20\334\301\34\1\30\334$M\21%L>\13\60\317BD\354\4L\4\12\0\12f\0\206"
	"\21\210a\246\66\30\7v\10\207y\230\7\67\240\205r\300\7z\250\7y(\7\71 \5>\260\207r\30\7zx\7y\340\3s`\207w\10\7z`\3\60\240\3?\0\3?\320"
	"\3=h\207t\200\207y\370\5z\310\7x(\7\24\0\63\211\301\70\260C8\314\303<\270\1-\224\3>\320C=\310C9\310\1)\360\201=\224\303\70\320\303"
	";\310\3\37\230\3;\274C8\320\3\33\200\1\35\370\1\30\370\1\22\210\202\214\231\322`\34\330!\34\346a\36\334\200\26\312\1\37\350\241\36"
	"\344\241\34F\241\36\304\241\34BA\36\344!\34\362\201\17\354\241\34\306\201\36\336A\36\370\300\34\330\341\35\302\201\36\330\0\14\350"
	"\300\17\300\300\17P@\20\222\4\304\60\302\60\234\31\14\227\17,\210\321\60D3\371\13a\0\2\346KSD\11\223\317\302\300DD\234\323H\23j\224"
	"eX\206eX\206eX\206\3\203\341\362\201\5\61\32\206h&\177!\14@\300|!\200\201\211\210\70\247\221&\24\60\324\34\31\14\227\17,\210\321"
	"\60D3\371\13a\0\2\346KSD\11\223\317\246\333j:\3\261 B!\226\341\306`\270|`A\214\206!\232\311_\10\3\20\60_\10Lg \26\316i\244)\335V"
	"\24<\4\215\0\224@\242i\216\0)\306\0\0@\5\220U\14\11\0\200j \354\246\341\362'\354!$\177%\244\225\230\374\342\266Q1\14\303\0Pu\317"
	"p\371\23\366\20\222\37\2\315\260\20(\330\12\243\1\34\67\14\303\0\0\300@]A\6`\30\206a\30\350+\303\0\14\24\336\64\\\376\204=\204\344"
	"w\10C4\22\342\64\22\"\0\0\0\12\61\2\34@d!\6`\30\310<j\270\374\11{\10\311\347\66\252X\211\311/n\33\21\303\60\14\205@\1\16\240t\216"
	" (\6\7T\0\260\22;\20p\226\260\0\222\344\63\300\24!\227_,\16\60\371\270\217\243@9I\232\"J\230|\26\6&\"\342\234F\232P\363.\303\62,"
	"\303\62,\303\62\34$M\21%L>\233n\253\351\14\304\202\210\227X\206a\4a8G\232\"J\230\374\24\271\210ElPA\310\262\274\34#\4\60\60\21\21"
	"\347\64\322\204\2\370\36!0\235\201X8\247\221\246t[Q\20g\4\0\0\0\0\23\24r\300\207t`\207\66h\207yh\3r\300\207\15\257P\16m\320\16zP"
	"\16m\0\17z0\7r\240\7s \7m\220\16q\240\7s \7m\220\16x\240\7s \7m\220\16q`\7z0\7r\320\6\351\60\7r\240\7s \7m\220\16v@\7z`\7t\320\6"
	"\346\20\7v\240\7s \7m`\16s \7z0\7r\320\6\346`\7t\240\7v@\7m\340\16x\240\7q`\7z0\7r\240\7v@\7:\17\244\220!#ED\0\206\1\0\323\14\0\230"
	"f\0\300\64\3\0&\32\0\60\233\0\0\10\5\0\60\344\231\200\0\0\0\0\0\0\0\0\0`\310c\1\1\20\0\0\0\0\0\0\0\300\220\7\3\2@\0\0\0\0\0\0\0\200"
	"!O\7\4\300\0\0\0\0\0\0\0\0C\236\17\10\200\0\0\0\0\0\0\0\0\206<a\0\4@\0\0\0\0\0\0\0\0C\36\62\0\2`\0\0\0\0\0\0\0\200!\217\31\0\1\20"
	"\0\0\0\0\0\0\0\300\220'\15\200\0\30\0\0\0\0\0\0\0`\310\303\6@\0\14\0\0\0\0\0\0\0\220\5\2(\0\0\0\62\36\230\34\31\21L\220\214\11&G"
	"\306\4C\2J`\4\240\30\212\240$\312\240\200\5\12\70\240\20\312\241\0\3\12\63\240@\3\12\67\240\260\3\12<\240\360\3\12P\240\60\5\12T"
	"\240p\5\12\245@J\241\360\12\246\200\12\254<\250*\220\"\30\1(\204\62(\11Jf\0\210\231\1\240f\6\200\234\31\0zf\0\10\232\1\240h\6\200"
	"\220\31\0:f\0\310\230\1\240b\6\200\210\31\0\22f\0\10\236\1\240x\6\200\346\31\0\242g\0\250\236\1 {\254\350!\216\3\0\216\343\70\216"
	"\3\0\216\343\0\0y\30\0\0b\1\0\0\32\3L\220F\2\23D5\30c\13s;\3\261+\223\233K{s\3\231q\271\1A\241\13;\233{\221*b*\12\232*\372\232\271"
	"\201y1Ks\13cK\331\20\4\23\4 \232 \0\322\6a 6\10\4A\1nn\202\0L\33\206\3!&\10\7\37\360\270\12\263+\243*\303\233 \0\324\4\1\250\66\10"
	"D\263!!\224\205 \6\206p\10A\261\205\271\205\311I\225\231\261\225\215\321\245\275\271mH\6\5\"\206\201!\34>WaterNormalTile\33\22F\221"
	"\10f`\10g\202\0X\23\204\204\16h\24\271\331Q\225\341mH\250\312\"\250\241!\234\15\304\23M\327\4\301\271\3\62cbWaterConst\23\4\340\332"
	"\200\20\231F\20\303\6L\20\"=\340\63&\326\364FV\306\66\364\346\66G\7%$5A\0\260\15\310\320y\304\60|\300\6\201\3\203\11\202\0\12d\206"
	"\336\330\336\344\246\302\332\340\330\312\344\66 \204\30\214\1A\14\4\260! \203\15\4\6\204A\31L\20\340 \24\70\264\331\301M\20\200l"
	"\303\220\6i0L\20\0m\3\202\240\1\243\6\24\261\6\15\215\261\260\66\250\267\271\11\2\260m0\220\66\240\334`\15\32\62streamOffset\23\4"
	"\200\333` p@\305\301\32\64\64\304\336\360\232\302\360&\10@\267\301@\346\200\242\203\65hH\300\205\221\311L\20\0o\203\201\330\1u\7"
	"k\320\320\20{\303kJs\333` y@}k\320\220\200\13#\233\231 \0\337\6\3\331\3\212\17\326\240\241\62V'V6T\346FW&7A\0\300`\203\201\370\1"
	"\365\7k\320\360\271\13\263+{23\233+\243\233\32\13c+\233 \0a\260\301@B\201\22\205\65h\230\210\275\341\65\205\341\31\205\311M\20\0"
	"\61\330` \244@\225\302\32\64$\340\302\310j&\10\300\30l0\220S\240Pa\15\32&boxMinFar\23\4\200\14\66\30\210*P\253\260\6\15\11\270\60"
	"\62\233\11\2P\6\33\14\244\25(WX\203f\203\263\261\301\33\310A\35\340\201\36\364\1(\214\202)\244\2+\274\302\4!\16D\201E\333\33Y\31"
	"\333\6\4\221\5F\15(b\15\32\"material\33\14\204\26(7X\203\206\211X\330\\\331\320\33\333\233\334\6\3\261\5\212\16\326\240\341\0\27"
	"F\266\301@p\201\272\203\65h6\24\337,\324\302-\344\302\4\301\350\203\11\2`\6\14\320\66\30H/P\304\32\64$\332\322\340\346&\10\300\31"
	"l \220_\240\300a\202@\374\301\6\201\20\207\15\301\70l\20\10r\330@\360\202/\204C9L\20\12?\240\201\26\346F\306V6A\0\320`\203\201\240"
	"\3E\254A:l\20\322@\35&\10\310\36l\30\300\301\27\312a\202\60\7\243\60A\0\322\200\3\334\333\34\227)\253/\250\267\271\64\272\264\67"
	"\267\15\10\2\17\24\221\6\361\260\6\15\205:;&TexCoord0\33\20d\36(pH\3zX\203\206\307]\230]Y\225U\314\4\1P\3&TexCoord1\33\20\304\36"
	"\250{H\3|X\203\206\307]\230]Y\225\225\314\4\1X\3&TexCoord2\33\20D\37\250}H\3~X\203\206\4\334\333\334\325\4\1`\3&Position0\33\20\304"
	"\37\250\177H\3\220X\203f\203\361\16\362P\17\371\320\17!1A\240\3R\240r\25FW&7\364\346\66G\267\201@H\202\"6\10[IL\20\352\240\24\330"
	"\64\275\221\225\261\15\275\271\315\321A\11Im \20\224\240\210\15\302\227\22\23\4\246\16&\10\215\35L\20\36<\230 @y\260!#\316\0\26b"
	"A\27v\301\34\316a\35\330\241\35\334A$F\302$NB%V\2\26X\302$ZB\27\\B%&\10\324\261\1\330\60\20\61\21\23\33\2\231\330\60\14\60\61\23"
	"\23\4;0\205\15AM\220h\13Ks\233 Ts0A\250\344`C@L\20\252\70\230 Tp\260A\240\250\15\13\21\17\70\221\23:\261\23\303N\20<\321\23D\250"
	"\312\360\206\336\336\344\310&\10\325\33lX\206\237\300\11\236\320\11\260\30\300b\340\211\236\330\20\14\33\26\346'p\202'\304\2,\6\260"
	"\30\300\242'6\4\314\206\205\372\11\234\340\11\262\0\213\1,\30\236\350\11\"Position\33\4j\15\66,i`\26\70\301\23:\1\26CNP<q\26\33\12"
	"\237\10\213\261(\13\264`2e\365E\25&wVF7A\250\334`\202\0\264\301\6\201b\213\15\13\241\26\70\261\26:\301\23\303N\20<\321\26\33\2\267"
	"\330\60\244\305[\0\33\12\230\270\11\270\60\3\240\12\33\233]\233K\32Y\231\33\335\224 \250B\206\347bW&7\227\366\346\66% \232\220\341"
	"\271\330\205\261\331\225\311M\11\212:dx.shaderModelS\2\244\14\31\236\213\\\331\334[\235\334X\331\334\224\240\14*\221\341\271\320"
	"\345\301\225\5\271\271\275\321\205\321\245\275\271\315M\21\\b&\352\220\341\271\330\245\225\335%\221M\321\205\321\225M\11j\242\16"
	"\31\236K\231\33\235\\\36\324[\232\33\335\334\224\0.\0y\30\0\0L\0\0\0\63\10\200\34\304\341\34f\24\1=\210C8\204\303\214B\200\7yx\7"
	"s\230q\14\346\0\17\355\20\16\364\200\16\63\14B\36\302\301\35\316\241\34f0\5=\210C8\204\203\33\314\3=\310C=\214\3=\314x\214tp\7{\10"
	"\7yH\207pp\7zp\3vx\207p \207\31\314\21\16\354\220\16\341\60\17n0\17\343\360\16\360P\16\63\20\304\35\336!\34\330!\35\302a\36f0\211"
	";\274\203;\320C9\264\3<\274\203<\204\3;\314\360\24v`\7{h\7\67h\207rh\7\67\200\207p\220\207p`\7v(\7v\370\5vx\207w\200\207_\10\207"
	"q\30\207r\230\207y\230\201,\356\360\16\356\340\16\365\300\16\354\60\3b\310\241\34\344\241\34\314\241\34\344\241\34\334a\34\312!\34"
	"\304\201\35\312a\6\326\220C9\310C9\230C9\310C9\270\303\70\224C8\210\3;\224\303/\274\203<\374\202;\324\3;\260\303\14\304!\7|p\3z("
	"\207v\200\207\31\321C\16\370\340\6\344 \16\347\340\6\366\20\16\362\300\16\341\220\17\357P\17\364\0\0\0q \0\0e\0\0\0\5\20\6\201\337"
	"\360\315~\313\247\341\66\234]\226\3\201\263\352\64\334\206\263\313\362)=L/\3\201\301\"\220\203\300\257\350\266S\313x pf\375\221\250"
	"e<\275./\17\353\342rP.\17\347\201\326\37\311^\36\323\337r`\223\4\233\1\201@`\260\4\352 \360\203\262\303\356\260\\Z6\263\313cz\372"
	"\355\6\2g\326\37\211Z\306\323\353\362\262\214\10\264\376H\366\362\230\376\226\3\233$\330\14\10\4\2\203\65@\7\201\337\65L/\313\235"
	"oy;\314\246\246\331e pf\375\221\250e<\275./\313\210@\353\217d/\217\351o9\260I\202\315\200@ 0X\1l\20\370]\303\366\62\265\214\7\2g"
	"\326\37\211Z\306\323\353\362\262\214\10\264\376H\366\362\230\376\226\3\233$\330\14\10\4\2\203f`\11\201\351\14\304\302\71\215\64\245"
	"\333\372/\204\1\10\230\25LB\0\3\23\21qN#M\377B\30\200\200\231\3\64\\\276\363\370\1\322\0\21\346\27\267m\17\333p\371\316\343\13\1"
	"U\24DT:\300P\22\6 `~q\333\6!\15\227\357<\276\20\21\300D\204@3,\204-8\303\345;\217?8\323\355\27\267m\15\323p\371\316\343\33\304\324"
	"!\14\321H\210\323H\206\60\15\227\357<\376\342\0\203\330<\324\344\27\267m\12\320p\371\316\343K\0\363,\204_\334\266%T\303\345;\217"
	"/MND\240\324\364P\223_\334\266\61<\303\345;\217O5@\204\371\305m\333\1\301\0H\3\0\0\0\0\0DXIL\10\24\0\0`\0\0\0\2\5\0\0DXIL\0\1\0\0"
	"\20\0\0\0\360\23\0\0BC\300\336!\14\0\0\371\4\0\0\13\202 \0\2\0\0\0\23\0\0\0\7\201#\221A\310\4I\6\20\62\71\222\1\204\14%\5\10\31\36"
	"\4\213b\200\30E\2B\222\13B\304\20\62\24\70\10\30K\12\62b\210H\220\24 CF\210\245\0\31\62B\344H\16\220\21#\304PAQ\201\214\341\203\345"
	"\212\4\61F\6Q\30\0\0\10\0\0\0\33\214\340\377\377\377\377\7@\2\250\15\204\360\377\377\377\377\3 m0\206\377\377\377\377\37\0\11\250"
	"\0I\30\0\0\3\0\0\0\23\202`B L\10\6\0\0\0\0\211 \0\0\214\0\0\0\62\"\210\11 d\205\4\23#\244\204\4\23#\343\204\241\220\24\22L\214\214"
	"\13\204\304L\20\304\301\10@\11\0\12f\0\346\10\300`\216\0)\306@\20DA\220Q\14\200 \210b \344\246\341\362'\354!$\177%\244\225\230\374"
	"\342\266Q1\14\303@Pq\317p\371\23\366\20\222\37\2\315\260\20(X\12\243\20\14\63\14\303@\20\304@MA\6b\30\206a\30\350)\303@\14\24\335"
	"\64\\\376\204=\204\344w\10C4\22\342\64\22\"\10\202 \12\61\21\14AT!\6b\30\310:j\270\374\11{\10\311\347\66\252X\211\311/n\33\21\303"
	"\60\14\205\300\10\206\240l\216 (\6C\24\4\261\21\67\20\60\214@\14\63\265\301\70\260C8\314\303<\270\1-\224\3>\320C=\310C9\310\1)\360"
	"\201=\224\303\70\320\303;\310\3\37\230\3;\274C8\320\3\33\200\1\35\370\1\30\370\201\36\350A;\244\3<\314\303/\320C>\300C9\240\200\230"
	"I\14\306\201\35\302a\36\346\301\15h\241\34\360\201\36\352A\36\312A\16H\201\17\354\241\34\306\201\36\336A\36\370\300\34\330\341\35"
	"\302\201\36\330\0\14\350\300\17\300\300\17\220\360\201\24\316\224\6\343\300\16\341\60\17\363\340\6\264P\16\370@\17\365 \17\345\60"
	"\12\365 \16\345\20\12\362 \17\341\220\17|`\17\345\60\16\364\360\16\362\300\7\346\300\16\357\20\16\364\300\6`@\7~\0\6~\200\202\217"
	"\306$\370\206\21\206\341\314`\270|`A\214\206!\232\311_\10\3\20\60_\232\"J\230|\26\6&\"\342\234F\232PS\232\203\71\230\203\71\230\203"
	"\71\34\30\14\227\17,\210\321\60D3\371\13a\0\2\346\13\1\14LD\304\71\215\64\241\340$\364\310`\270|`A\214\206!\232\311_\10\3\20\60_"
	"\232\"J\230|6\335V\323\31\210\5\21\345g\16\67\6\303\345\3\13b4\14\321L\376B\30\200\200\371B`:\3\261pN#M\351\266\242@\245\365&i\212"
	"(a\362Y\200y\26\"b'`\"P@P\233\21\304\24\0\0\0\0\0\23\24r\300\207t`\207\66h\207yh\3r\300\207\15\257P\16m\320\16zP\16m\0\17z0\7r\240"
	"\7s \7m\220\16q\240\7s \7m\220\16x\240\7s \7m\220\16q`\7z0\7r\320\6\351\60\7r\240\7s \7m\220\16v@\7z`\7t\320\6\346\20\7v\240\7s "
	"\7m`\16s \7z0\7r\320\6\346`\7t\240\7v@\7m\340\16x\240\7q`\7z0\7r\240\7v@\7C\236\0\0\0\0\0\0\0\0\0\0\0\206<\6\20\0\1\0\0\0\0\0\0\0"
	"\14y\20 \0\4\0\0\0\0\0\0\0\30\362\64@\0\14\0\0\0\0\0\0\0\60\344y\200\0\10\0\0\0\0\0\0\0`\310\23\1\1\20\0\0\0\0\0\0\0\300\220\207"
	"\2\2`\0\0\0\0\0\0\0\200!\217\5\4@\0\0\0\0\0\0\0\0C\236\14\10\200\1\0\0\0\0\0\0\0\206<\34\20\0\3\0\0\0\0\0\0\0d\201\0\23\0\0\0\62"
	"\36\230\24\31\21L\220\214\11&G\306\4C\"J`\4\240\20\212\241\10J\242\14\12X\240\200\3\250(\211\62(\204\21\200\"(\20\22g\0\210\234\1"
	"\240t\6\200\330\31\0rg\0\350\35+z\210\343\0\200\343\70\216\343\0\200\343\70\0y\30\0\0\213\0\0\0\32\3L\220F\2\23D5\30c\13s;\3\261"
	"+\223\233K{s\3\231q\271\1A\241\13;\233{\221*b*\12\232*\372\232\271\201y1Ks\13cK\331\20\4\23\4B\231 \20\313\6a &\10\4\263A\30\14\12"
	"ps\23\4\242\331\60 \11\61A\20\203\214\300\4\201p&\10\304\263A \234\15\11\261\60\4\61\64\304\263!\31\26\206\30\206\206x6$\315\302"
	"\20\315\320\20\317\4\201\200&\10d\240mH&\212!\246\301!\236\15\4\24I\325\4!\15\266\11\2\21m@\210\213!\210\1\3&\10l\300M\20\10i\3\62"
	"h\14\61\14\33\260A\310\270\11\202\33t\33\20\302c\10b \200\15\301\267\201\260\200\16\14&\10\2\260\1\330\60\20c0\6\33\2\62\330\60\14"
	"bP\6\23\204\67\360\66\4g@\242-,\315\215\313\224\325\27\324\333\\\32]\332\233\333\4\241\230&\10\5\265! &\10E5A(\254\15\13\241\6k\300"
	"\6m\340\6\203\33\20o\0\20\241*\303\33z{\223#\233 \24\327\206e\210\203\65x\203\66\220\203A\16\206\67\0\66\4\303\206\245\211\203\65"
	"x\3:\220\203A\16\6\71\0\66\4\315\206e\212\203\65x\3;\220\203A\16\232\67\0\210@\275\315\245\321\245\275\271mX\14<X\203\67h\3\71\30"
	"\330`z\3`C\1\7sP\7w\220\7L\246\254\276\250\302\344\316\312\350&\10\5\266a!\366`\15\370\240\15\336`p\3\342\15\200\15A\37l\30\364\300"
	"\17\200\15\205\30\244\301\37\204\1P\205\215\315\256\315%\215\254\314\215nJ\20T!\303s\261+\223\233K{s\233\22\20M\310\360\\\354\302"
	"\330\354\312\344\246\4F\35\62<\227\71\264\60\262\62\271\246\67\262\62\266)AR\206\14\317E\256l\356\255Nn\254lnJ\0\6\225\310\360\\"
	"\350\362\340\312\202\334\334\336\350\302\350\322\336\334\346\246\4eP\207\14\317\305.\255\354.\211l\212.\214\256lJp\6u\310\360\\\312"
	"\334\350\344\362\240\336\322\334\350\346\246\4\177\0\0\0\0y\30\0\0L\0\0\0\63\10\200\34\304\341\34f\24\1=\210C8\204\303\214B\200\7"
	"yx\7s\230q\14\346\0\17\355\20\16\364\200\16\63\14B\36\302\301\35\316\241\34f0\5=\210C8\204\203\33\314\3=\310C=\214\3=\314x\214tp"
	"\7{\10\7yH\207pp\7zp\3vx\207p \207\31\314\21\16\354\220\16\341\60\17n0\17\343\360\16\360P\16\63\20\304\35\336!\34\330!\35\302a\36"
	"f0\211;\274\203;\320C9\264\3<\274\203<\204\3;\314\360\24v`\7{h\7\67h\207rh\7\67\200\207p\220\207p`\7v(\7v\370\5vx\207w\200\207_\10"
	"\207q\30\207r\230\207y\230\201,\356\360\16\356\340\16\365\300\16\354\60\3b\310\241\34\344\241\34\314\241\34\344\241\34\334a\34\312"
	"!\34\304\201\35\312a\6\326\220C9\310C9\230C9\310C9\270\303\70\224C8\210\3;\224\303/\274\203<\374\202;\324\3;\260\303\14\304!\7|p"
	"\3z(\207v\200\207\31\321C\16\370\340\6\344 \16\347\340\6\366\20\16\362\300\16\341\220\17\357P\17\364\0\0\0q \0\0&\0\0\0v\0\15\227"
	"\357<~\200\64@\204\371\305m\33\302\66\\\276\363\370B@\25\5\21\225\16\60\224\204\1\10\230_\334\266%H\303\345;\217/D\4\60\21!\320\14"
	"\13a\4\316p\371\316\343\17\316t\373\305m\233\301\64\\\276\363\370\6\61u\10C4\22\342\64\222\5L\303\345;\217\277\70\300 6\17\65\371"
	"\305m\333\0\64\\\276\363\370\22\300<\13\341\27\267m\2\325p\371\316\343K\223\23\21(5=\324\344\27\267m\5\317p\371\316\343S\15\20a~"
	"q\333\6@0\0\322\0\0\0a \0\0\350\2\0\0\23\4F,\20\0\0\0+\0\0\0\264\215\0PQ\2D\24DA\6\224C\301\224J\271\224L\341\225B\311\25R\331\225"
	"Q\271\315\0\320\60F \213n\177\177c\4\67\336\216-7F\240\342\365)~c\4 \10\202\360\67F\0\202 H\177c\4\243\333\307\363\67F\300\262~\251"
	"~c\4 \10\202 \30\214\21\274\261\33\273\337\30\1\10\202\240\12\6c\4 \10\202\370/\214\21\230s\316\332\337\30\201\212\327\247\354\215"
	"\21\200 \10\302\277\60F \363kI\377\302\30\301\211\263s\374\215\21\234\277\254\237\337\30\201\316\232s\375\215\21\200 \10\202\240"
	"\60\2\60\3\60F\0\202 \210\177\0#\6\11\0\202`\300\365\1\7\7v`\7g0b\220\0 \10\6\234\37tq\220\7y\200\6#\6\11\0\202`\300\375\201'\7~"
	"\340\7i0b\220\0 \10\6\34(|s\340\7~\240\6#\6\11\0\202`\300\205\2\30\330\301\37\374\301\32\214\30$\0\10\202\1'\12a@\7\241\20\12l0b"
	"\220\0 \10\6\334(\210A\35\204B(\264\301\210A\2\200 \30\30\254\20\12\241 \12w\340\215\30$\0\10\202\201\321\12\242 \12\243\340\6\337"
	"\210A\2\200 \30\30\256\60\12\243@\12x\0\6#\6\11\0\202``\274\2)\200B)\350A\30\214\30$\0\10\202\201\1\13\245\20\12\246\20\7b0b\220"
	"\0 \10\6F,\230\2)\234\2\37\214\301\210A\2\200 \30\30\262p\12\245\200\12s@\6#\6\11\0\202``\314\2*\250B*\370A\31\214\30$\0\10\202\201"
	"A\13\251\260\12\252P\7f0b\220\0 \10\6F-\250\302*\254\2(\234\301\210A\2\200 \30\30\266\260\12\254\300\12w\200\6#\6\7\0\202`\220\301"
	"\2\33\64o0\232\20\0\243\11B0\232\60\10#\6\17\0\202`\320\344\2\35L\22r`X,\304B\33`\243\11\201\60\232 \14&\210\1|L\30\3\370\214\30"
	",\0\10\202\301\323\13s \224A \224A0b`\0 \10\6Q/\354A`D \37\23\320@>V\14\362\31\61\70\0\20\4\203\314\27\364@\323\205\321\204\0\30"
	"M\20\202\321\204A\260n\210\217uC|\254\33\342\63b\260\0 \10\6O:\374\301 \4\203\20\214\30\30\0\10\202A\224\16\247\20X\21\310\307\12"
	"A>V\14\362\31\61\70\0\20\4\3\212\35`A\15\316a4!\0.\30l4a\10.\30l\304`\1@\20\14\236yH\5\344\60\250I\262 \220\217\321A\20\37\13JA>"
	"f\7A|\206#\202<\30>s\370@>\346\364\201|\314\361\3\371\214\30\30\0\10\202A\264\17\274P\230\2\12\362\261@\200\217\5` \37\23\300@>6"
	"\200\201|,\31\342c\311\20\37K\206\370L7\14\246\320L7\14\247\340L7\14\250\360\214\30,\0\10\202\301\203\22\276\60\10\301 \4#\6\6\0"
	"\202`\20\241\204\71\4V\4\362\261B\220\217\25\203|F\14\16\0\4\301 K\211r(\5t\30M\10\200\321\4!\30M\30\4\33P!>6\240B|l@\205\370\330"
	"\240\320\307\6\205>6(\364\31\61\70\0\20\4\203l&\336\341\25\\b4!\0F\23\204`4a\20l\220\205\370\330 \13\361\261A\26\342c\3E\37\33(\372"
	"\330@\321g\304\340\0@\20\14,\237\300\207f\30\61\70\0\20\4\3\353'\362\241\31F\14\16\0\4\301\300\2\13}h\206\21\203\3\0A0\260\302\242"
	"\37\6a\304\340\0@\20\14,\261\360\207@\260\200\223\217\11\234|l\340\344c\3\70\300\307\6p\200\217\15\340\0\237\21\203\3\0A0\310\320"
	"\202$\310\241%F\23\2`4A\10F\23\6\301\216!>v\14\361\261c\210\317\210\301\3\200 \30\64t\361\22\361\320\16\203\20\320\3J\240\4J\314"
	"\303hB\0\214&\10\301h\302 \330 \16\362\261A\34\344c\203\70\310g\304`\1@\20\14\36\275\200\211]\320\205\\\330\207|\330\207\21\3\3\0"
	"A0\210\364B.\202\341\210\0&\210o\304\340\0@\20\14\262\274\250\211z\360\211Y\206\200\20l\25VB>\26\200\203|L\0\7\371\330\0\16\362\261"
	"[\30\342c\267\60\304\307na\210\317\210\301\2\200 \30<\245\261\23\203\20\14B0b`\0 \10\6Qi\214E`E \37+\4\371X1\310g\304\340\0@\20\14"
	"\62\323\20\13\221(\213\321\204\0\30M\20\202\321\204A\260\241$\342cCI\304\307\206\222\210\217\15\12}lP\350c\203B\237\21\203\3\0A0"
	"\310`\203-Xb5F\23\2`4A\10F\23\6\301\206\227\210\217\15/\21\37\33^\">6P\364\261\201\242\217\15\24}F\14\16\0\4\301\300\332\215\272"
	"h\206\21\203\3\0A0\260x\303.\232a\304\340\0@\20\14\254\336\270\213f\30\61\70\0\20\4\3\313\67\364b\20F\14\16\0\4\301\300\372\215\275"
	"\10\4\13\70\371\230\300\311\307\6N>6\364\4|l\350\11\370\330\320\23\360\31M`\3`4\241\15\202\321\4\67\20\314\30\342c\306\20\37\63\206"
	"\370\214\30<\0\10\202A\3\37\253\321\26i1\10\1\\\220\6i\220\306[\214&\4\300h\202\20\214&\14\302h\2\61\14G\4\247A|\263\14\302\20\214"
	"\30\34\0\10\202A\26\37\255\321\26\270\61\232\20\0\243\11B0\232\60\10\66\300E|l\200\213\370\330\0\27\361\261\241\16\350cC\35\320\307"
	"\206:\240\317\210\301\1\200 \30d\373q\33wA\37\243\11\1\60\232 \4\243\11\203`\203^\304\307\6\275\210\217\15z\21\37\33\376\200>6\374"
	"\1}l\370\3\372\214\30\34\0\10\202\201e\"\340\321\14#\6\7\0\202``\235Hx4\303\210\301\1\200 \30X(\"\36\315\60bp\0 \10\6V\212\224\307"
	" \214\30\34\0\10\202\201\245\"\346\21\10\26\234\202|L8\5\371\330p\12\362\261\1\65\340c\3j\300\307\6\324\200\217\15k\20\37\33\326"
	" >6\254A|F\14\36\0\4\301\240\301\221\371\250\215\330\30\204\300\66\330\203=\330\343\66F\23\2`4A\10F\23\6a\226`\30\250\30\14\301\26"
	"\202\201\212\301\20l!\30\250\30\14\301\26\2\33\316C>6\240\207|lH\17\371\330x\330E|\214<\354\">V\36v\21\237\21\203\5\0A0x\310D?\6"
	"!(\15\322\30\15s\207#>\346\16G|\314\35\216\370\30\61\310\307\212A>6\30\362\261\241\201\217\15\15|lh\340\63KP\214\30\34\0\10\202A"
	"\246&&b\36f2\232\20\14\26\210\7|L\20\17\370\214\30<\0\10\202A3'.\362\36\354!\4\362!\37k\262&'\"\37\243\11\1\60\232 \4\243\11\203"
	"`\207z\300\307\20\365\200\317\210\301\3\200 \30\64yB#\365!\37B\200\37\370\21'q\322\"\370\61\232\20\0\243\11B0\232\60\10\226\230\10"
	"|, \340c\12\212\300\307\202\2>\266\250\10|,0\340\63b\260\0 \10\6\217\250\340H1\4\305\20\214\30\30\0\10\202A$*`\22\330\21\310\307"
	"\14A>\243\11)1\330\20\310\307\6A>&\334\7|L\270\17\370\230\260\33\364\61A7\350\63b\360\0 \10\6\215\253\244I\212\234\210\20\264H\213"
	"\230\212\251\210I\213\214&\4\300h\202\20\214&\14\202\315\210y\304\307h\304<\342c5b\36\361\31\61X\0\20\4\203\207V\324d\20\202\372"
	"\240\217\371\60\237\70\342c>q\304\307|\342\210\217\15\204|l(\344c\203!\37\33\32\370\330\320\300\307\206\6>#\6\13\0\202`\360\370\12"
	"\235h=\222i=\222\215\30\30\0\10\202A\344+|\22X\300\311\307\4N>\263\4\305@\305\340\21\376\60\14T\14b@\210\304\60P1\230\1a\22\303@"
	"\305\240\20,4\14T\14\16\301B\303@\305\300\20,4\14T\14\32\241\17\303@\305\240\21\372\60\14T\14\32\241\17\303\210\301\2\200 \30<\353"
	"\22*o\2'qb\24\304\210\301\1\200 \30d\350B*eB.\243\11\201\60\232 \4\243\11\3`o\"\304\307\202D>&$\362\261!\221\217\35\207|,8\25\371"
	"X`\310\307\222G>\246<\362\261\345\221\217\21\3|\254\30\340c\306\0\37\13\25\31\211\217\215\212\214\304\307JEF\342\63b\260\0 \10\6"
	"\17\310\330\312 \4\203\20\214\30\30\0\10\202A\4\62\276\22XP\310\307\204B>6\24\362\31\61X\0\20\4\203\247dv\5\14>/UTeUF\14\16\0\4\301"
	"\300\32\231^\11Ne\304\340\0@\20\14,\222\1\227\300TF\14\26\0\4\301\340A\31_\31\3\61\10\3\62(\3\63\30\61\70\0\20\4\3\313d\300%P\225"
	"\21\203\3\0A0\260Nf\\\202T\31\61X\0\20\4\203ge\302\345U`%V\22\344\30\61\70\0\20\4\3+e\306%h\225\21\203\3\0A0\260T\306\\\2V\31\61"
	"X\0\20\4\203\307e\310E\15\326\200\15\230E\31\61\70\0\20\4\3\213e\314%\200\225\21\203\3\0A0\260Z&]\202W11\20\3\371X\360+\362\61\303"
	"\220\217\5\202|,\340\25\370X\255<\361\31\61\60\0\20\4\203\310f\352%\260`\\\344\63b`\0 \10\6\21\316\340K`A!\37\13t\5>\266+R|F\14\14"
	"\0\4\301 \342\231}\11,H\27\371\214\30\30\0\10\202A\344\63\376\22X\260\310\307\2p\201\217\35\360\"\37\13\4\371\330\34\4\362\261\71"
	"\20\344cs0\310g\304`\1@\20\14\236\263\351\27Q\10\5P(\3\62\30\3#\3Q\220\217\231\201(\310\307\204 >\226\6\244 \37C\203S\220\217\11"
	"A|l\15RA>\326\6\251 \37\23\202\370\230\2\6\362\31\61X\0\20\4\203GnP\6)\4\244\20L\20\344c\201\0\37#\203\200>V\6e \37\263\227 >\346"
	"\6n \37\13\4\371X@\300g\304\300\0@\20\14\"\275\331\231\300\202:\220\217\331\201\35\310\307\202C>\26$\360\31\61\60\0\20\4\203\350"
	"o\300&\260\340\16\344c\201\1\37\243\231\200>\366<\362\261`f\344c\301 \37\63\231;\210\317\210\201\1\200 \30D\247c6\201\5\64#\237\21"
	"\3\3\0A0\210R'm\2S\231 >\26\310\202|L\220\5\371\330 \13\362\261\241\200\217\15\6|l8\340c\3#\37\33\32\371\330\340\310\307\206\70\200"
	"\217\15q\0\37\33\342\0>6\264\203|lp\7\371\330\360\16\362\261Ag\340c\303\316\300\307\6\236\201\217\31\3}\314\30\350c\306@\237\21\3"
	"\3\0A0\210\300\347o\206\21\3\3\0A0\210\302\7t\206\21\3\3\0A0\210\304't\6\33\340F>6\304\215|l\220\33\371\214\30\30\0\10\202Ad>\246"
	"\63\214\30\30\0\10\202At>\247\63\214\30\30\0\10\202A\204>\250\63\214\30$\0\10\202\1\302>\245\63>\343\203;\303\210A\2\200 \30 \354"
	"S:\343\63>\256#\214\30$\0\10\202\1\302>\245\63>\343s;\301\210A\2\200 \30 \354S:\343\63>\272\243\67\10\0\0\0\0";

}; // namespace NWater