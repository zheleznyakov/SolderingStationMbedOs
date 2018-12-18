/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* Класс PowerControl представляет программную реализацию пятиканального фазового регулятора
* который представляет собой симисторный регулятор мощности из пяти симисторов. В этом регуляторе
* присутствует детектор 0, который выдает сигнал 3.3 вольта, как только синусоида пересекла 0.
* В этом момент срабатывает прерывание ZeroCross_, которое взводит пять таймеров на время от 0 до 10000мкс
* 10000мкс - время полупериода колебаний тока в розетке 50Гц. Чем позже сработает таймер, тем меньшую мощность 
* получим в нагрузке.
*/

#include "PowerControl.h"

PowerControl::PowerControl(PinName ZeroCross,PinName h1, PinName h2, PinName h3, PinName h4, PinName hup):ZeroCross_(ZeroCross), h1_(h1), h2_(h2), h3_(h3), h4_(h4), h5_(hup)
{
    // 120 - примерно полнакала (значение по умолчанию)
    setD();
    dim1 = 0;
    dim2 = 0;
    dim3 = 0;
    dim4 = 0;
    dimup = 0;
    
    ZeroCross_.rise(callback(this,&PowerControl::Crossing)); // инициализация прерывания для детектора 0

}
// Crossing() - обработчик прерывания от детектора 0; взводит пять таймеров для каждого из симисторов

void PowerControl::Crossing()
{
    //d[Q] - в массиве хранятся значения микросекунд для мощности Q, Q=0 - минимальная мощность, Q=250 - максимальная мощность
    t1.attach_us(callback(this,&PowerControl::DimHeater1UP),d[dim1]);
    t2.attach_us(callback(this,&PowerControl::DimHeater2UP),d[dim2]);
    t3.attach_us(callback(this,&PowerControl::DimHeater3UP),d[dim3]);
    t4.attach_us(callback(this,&PowerControl::DimHeater4UP),d[dim4]);
    t5.attach_us(callback(this,&PowerControl::DimHeater5UP),d[dimup]);
} 
// SetDimming задает мощность каждого из симисторов от 0 до 250
void PowerControl::SetDimming(int d1, int d2, int d3, int d4, int dup)
{
    dim1 = d1;
    dim2 = d2;
    dim3 = d3;
    dim4 = d4;
    dimup = dup;
}
//устанавливает значение мощности value для одного канала heaterNuber
void PowerControl::SetDimming(int heaterNumber,int value)
{
    if (heaterNumber==0)
        dim1=value;
    if (heaterNumber==1)
        dim2=value;
    if (heaterNumber==2)
        dim3=value;
    if (heaterNumber==3)
        dim4=value;
    if (heaterNumber==4)
        dimup = value;

}
// DimHeater1UP() обработчик таймера, включающий перывый симистор
void PowerControl::DimHeater1UP()
{
    if (heater[0]==0) return;
            h1_ = 1; // подаем на управлюящий первым симистором выход единицу
            t1.detach();// взводим таймер заново, чтобы через 300мкс снять управляющий сигнал с симистора
            t1.attach_us(callback(this,&PowerControl::DimHeater1Down),5);
}
// DimHeater1Down() обработчик преывания, отключающий управляющий сигнал первого симистора
// сам симистор отключится не сразу, а как только синусоида перейдет через 0 значение
void PowerControl::DimHeater1Down()
{
    h1_ = 0;
    t1.detach();
}
void PowerControl::DimHeater2UP()
{
    if (heater[1]==0) return;
            h2_ = 1;
            t2.detach();
            t2.attach_us(callback(this,&PowerControl::DimHeater2Down),5);
}
void PowerControl::DimHeater2Down()
{
    h2_ = 0;
    t2.detach();
}
void PowerControl::DimHeater3UP()
{
    if (heater[2]==0) return;
            h3_ = 1;
            t3.detach();
            t3.attach_us(callback(this,&PowerControl::DimHeater3Down),5);
}
void PowerControl::DimHeater3Down()
{
    h3_ = 0;
    t3.detach();
}
void PowerControl::DimHeater4UP()
{
    if (heater[3]==0) return;
            h4_ = 1;
            t4.detach();
            t4.attach_us(callback(this,&PowerControl::DimHeater4Down),5);
}
void PowerControl::DimHeater4Down()
{
    h4_ = 0;
    t4.detach();
}
void PowerControl::DimHeater5UP()
{
            h5_ = 1;
            t5.detach();
            t5.attach_us(callback(this,&PowerControl::DimHeater5Down),5);
}
void PowerControl::DimHeater5Down()
{
    h5_ = 0;
    t5.detach();
}
int PowerControl::ToggleHeater(int h)
{
    if (h>3||h<0)
        return 0;
    heater[h]=!heater[h];
    return heater[h];
}

/* PowerControl::setD() инициализирует массив D[Q]=t, где Q- мощность от 0 до 250(0-минимальная; 250-максимальная) t- задержка в микросекундах 
*  чем больше задержка, тем познее включится симистор, тем меньше мощность. Значения t посчитаны заранее, чтобы получить линейное приращение мощности
*  ссылка на статью https://habr.com/post/145991/
*/
void PowerControl::setD(void)
{
    d[250]=0;
    d[249]=850;
    d[248]=1075;
    d[247]=1233;
    d[246]=1361;
    d[245]=1469;
    d[244]=1564;
    d[243]=1649;
    d[242]=1727;
    d[241]=1800;
    d[240]=1867;
    d[239]=1930;
    d[238]=1990;
    d[237]=2047;
    d[236]=2101;
    d[235]=2154;
    d[234]=2204;
    d[233]=2252;
    d[232]=2298;
    d[231]=2343;
    d[230]=2387;
    d[229]=2429;
    d[228]=2471;
    d[227]=2511;
    d[226]=2550;
    d[225]=2589;
    d[224]=2626;
    d[223]=2663;
    d[222]=2699;
    d[221]=2734;
    d[220]=2768;
    d[219]=2802;
    d[218]=2836;
    d[217]=2868;
    d[216]=2901;
    d[215]=2932;
    d[214]=2964;
    d[213]=2995;
    d[212]=3025;
    d[211]=3055;
    d[210]=3085;
    d[209]=3114;
    d[208]=3143;
    d[207]=3171;
    d[206]=3200;
    d[205]=3227;
    d[204]=3255;
    d[203]=3282;
    d[202]=3309;
    d[201]=3336;
    d[200]=3363;
    d[199]=3389;
    d[198]=3415;
    d[197]=3441;
    d[196]=3466;
    d[195]=3492;
    d[194]=3517;
    d[193]=3542;
    d[192]=3567;
    d[191]=3591;
    d[190]=3616;
    d[189]=3640;
    d[188]=3664;
    d[187]=3688;
    d[186]=3711;
    d[185]=3735;
    d[184]=3758;
    d[183]=3782;
    d[182]=3805;
    d[181]=3828;
    d[180]=3851;
    d[179]=3874;
    d[178]=3896;
    d[177]=3919;
    d[176]=3941;
    d[175]=3964;
    d[174]=3986;
    d[173]=4008;
    d[172]=4030;
    d[171]=4052;
    d[170]=4074;
    d[169]=4096;
    d[168]=4117;
    d[167]=4139;
    d[166]=4160;
    d[165]=4182;
    d[164]=4203;
    d[163]=4224;
    d[162]=4246;
    d[161]=4267;
    d[160]=4288;
    d[159]=4309;
    d[158]=4330;
    d[157]=4351;
    d[156]=4371;
    d[155]=4392;
    d[154]=4413;
    d[153]=4434;
    d[152]=4454;
    d[151]=4475;
    d[150]=4495;
    d[149]=4516;
    d[148]=4536;
    d[147]=4557;
    d[146]=4577;
    d[145]=4597;
    d[144]=4618;
    d[143]=4638;
    d[142]=4658;
    d[141]=4678;
    d[140]=4699;
    d[139]=4719;
    d[138]=4739;
    d[137]=4759;
    d[136]=4779;
    d[135]=4799;
    d[134]=4819;
    d[133]=4839;
    d[132]=4859;
    d[131]=4879;
    d[130]=4899;
    d[129]=4919;
    d[128]=4939;
    d[127]=4959;
    d[126]=4979;
    d[125]=4999;
    d[124]=5019;
    d[123]=5039;
    d[122]=5059;
    d[121]=5079;
    d[120]=5100;
    d[119]=5120;
    d[118]=5140;
    d[117]=5160;
    d[116]=5180;
    d[115]=5200;
    d[114]=5220;
    d[113]=5240;
    d[112]=5260;
    d[111]=5280;
    d[110]=5300;
    d[109]=5321;
    d[108]=5341;
    d[107]=5361;
    d[106]=5381;
    d[105]=5402;
    d[104]=5422;
    d[103]=5442;
    d[102]=5463;
    d[101]=5483;
    d[100]=5504;
    d[99]=5524;
    d[98]=5545;
    d[97]=5565;
    d[96]=5586;
    d[95]=5607;
    d[94]=5628;
    d[93]=5648;
    d[92]=5669;
    d[91]=5690;
    d[90]=5711;
    d[89]=5732;
    d[88]=5753;
    d[87]=5775;
    d[86]=5796;
    d[85]=5817;
    d[84]=5839;
    d[83]=5860;
    d[82]=5882;
    d[81]=5903;
    d[80]=5925;
    d[79]=5947;
    d[78]=5969;
    d[77]=5991;
    d[76]=6013;
    d[75]=6035;
    d[74]=6058;
    d[73]=6080;
    d[72]=6103;
    d[71]=6125;
    d[70]=6148;
    d[69]=6171;
    d[68]=6194;
    d[67]=6217;
    d[66]=6240;
    d[65]=6264;
    d[64]=6288;
    d[63]=6311;
    d[62]=6335;
    d[61]=6359;
    d[60]=6383;
    d[59]=6408;
    d[58]=6432;
    d[57]=6457;
    d[56]=6482;
    d[55]=6507;
    d[54]=6533;
    d[53]=6558;
    d[52]=6584;
    d[51]=6610;
    d[50]=6636;
    d[49]=6663;
    d[48]=6690;
    d[47]=6717;
    d[46]=6744;
    d[45]=6772;
    d[44]=6799;
    d[43]=6828;
    d[42]=6856;
    d[41]=6885;
    d[40]=6914;
    d[39]=6944;
    d[38]=6974;
    d[37]=7004;
    d[36]=7035;
    d[35]=7066;
    d[34]=7098;
    d[33]=7130;
    d[32]=7163;
    d[31]=7197;
    d[30]=7231;
    d[29]=7265;
    d[28]=7300;
    d[27]=7336;
    d[26]=7373;
    d[25]=7410;
    d[24]=7449;
    d[23]=7488;
    d[22]=7528;
    d[21]=7569;
    d[20]=7612;
    d[19]=7656;
    d[18]=7701;
    d[17]=7747;
    d[16]=7795;
    d[15]=7845;
    d[14]=7897;
    d[13]=7952;
    d[12]=8009;
    d[11]=8069;
    d[10]=8132;
    d[9]=8199;
    d[8]=8271;
    d[7]=8350;
    d[6]=8435;
    d[5]=8530;
    d[4]=8638;
    d[3]=8765;
    d[2]=8924;
    d[1]=9148;
    d[0]=9901;
}

