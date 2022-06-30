#include "playscene.h"
#include"shapedwindow.h"
#include"mypushbutton.h"

playscene::playscene(QWidget *parent) : QWidget(parent)
{
    setFixedSize(960,720);
    battelbgm=new QSound(":/playscene/res/battelbgm.wav",this);
    battelbgm->setLoops(-1);
    connect(this,&playscene::gamestart,[=](){
        QSound::play(":/playscene/res/readysetplant.wav");

        //设置开场动画
        QLabel *ready=new QLabel(this);
        QLabel  *mask=new QLabel(this);
        mask->setFixedSize(size());
        QPixmap readypix(":/playscene/res/StartReady.png");
        ready->resize(readypix.size());
        ready->setPixmap(readypix);
        ready->move((width()-ready->width())*0.5,(height()-ready->height())*0.5);
        mask->show();
        ready->show();

        QTimer::singleShot(620,[=](){
            QPixmap setpix(":/playscene/res/StartSet.png");
            ready->resize(setpix.size());
            ready->setPixmap(setpix);
            ready->move((width()-ready->width())*0.5,(height()-ready->height())*0.5);

            QTimer::singleShot(620,[=](){
                QPixmap palntpix(":/playscene/res/StartPlant.png");
                ready->resize(palntpix.size());
                ready->setPixmap(palntpix);
                ready->move((width()-ready->width())*0.5,(height()-ready->height())*0.5);
                QTimer::singleShot(800,[=](){
                    delete ready;
                    delete mask;
                    battelbgm->play();
                    /***************************************************************************************************/
                    //僵尸死亡监视器以及僵尸随机生成器
                            timer = new QTimer;
                            connect(timer,&QTimer::timeout,[=](){
                                for(int i=0;i<ZombieNumber;++i)
                                {
                                    if(zombie[i]->hp<=0)
                                    {
                                        zombie[i]->Die(zombie[i]->DiePath);
                                        if(zombie[i]->movie->frameCount()==zombie[i]->movie->currentFrameNumber()+1)
                                        {
                                            zombie[i]->timer->disconnect();
                                            delete zombie[i];
                                            int n=i;
                                            ++n;
                                            for(;n<ZombieNumber;++n)
                                                zombie[n-1]=zombie[n];
                                            --ZombieNumber;
                                            --i;
                                        }
                                    }
                                }
                            });
                            timer->start(10);
                            zombie = new Zombies*[MaxNumber];
                            Ctimer = new QTimer;
                            connect(Ctimer,&QTimer::timeout,[=](){
                                Born(FCzombie[Zcnt%10],FCraw[Zcnt%10]);
                                ++Zcnt;
                            });
                            Ctimer->start(8000);//8秒生成一个僵尸
                    /***************************************************************************************************/
                });
            });
        });




    });


    //设置种子包
    QPixmap seedbank(":/playscene/res/Shop.png");
    QLabel* bank=new QLabel(this);
    bank->resize(seedbank.size());
    bank->setPixmap(seedbank);
    bank->move(200,0);
    bank->show();
    sunlabel=new QLabel(QString::number(sunnum),this);
    sunlabel->setAlignment(Qt::AlignCenter);
    sunlabel->move(bank->x()+27,65);
    sunlabel->show();
    for(int i=0;i<5;i++)
    {
        seedBank[i]=new Seed(bank,cooldownsequence[i],sunsequence[i],":/playscene/res/SunFlower.png");
        seedBank[i]->checksun(sunnum);
        connect(seedBank[i],&Seed::clicked,[=](){
            if(sunnum>=seedBank[i]->sun)
            {
                QSound::play(":/playscene/res/seedlift.wav");
                setCursor(QCursor(seedBank[i]->plantpic));
                for(int i=0;i<5;i++)
                {
                    if(!seedBank[i]->incd)
                        seedBank[i]->checksun(sunnum);
                }
                seedBank[i]->mask1->show();
                isPlanting=i;


            }
        });
        connect(seedBank[i],&Seed::check,[=](){
                    seedBank[i]->checksun(sunnum);
        });
        seedBank[i]->move(67+55*i,5);
        seedBank[i]->show();
    }

    //设置铲子
    shovel=new Shovel(this);
    shovel->move(bank->x()+bank->width()+20,0);
    shovel->show();
    connect(shovel,&Shovel::clicked,[=](){
        setCursor(QCursor(shovel->shovel));
        QSound::play(":/playscene/res/shovel.wav");
        shovel->setIcon(shovel->picture2);
        isPlanting=-2;

    });

    //设置菜单按钮
    MyPushButton* menu=new MyPushButton(this,true,":/playscene/res/MenuButton.png");
    qDebug()<<size();
    menu->move(this->width()-menu->width(),0);
    menu->show();
    connect(menu,&MyPushButton::clicked,[=](){
        /***************************************************************************************************/
        //僵尸暂停
        for(int i=0;i<ZombieNumber;++i)
            zombie[i]->Zstop();
        timer->disconnect();
        Ctimer->disconnect();
        /***************************************************************************************************/


        QSound::play(":/playscene/res/pause.wav");
        ShapedWindow* option=new ShapedWindow(this,":/menu/res/Options.png");
        option->move((this->width()-option->width())*0.5,(this->height()-option->height())*0.5);
        //返回按钮
        MyPushButton* Return=new MyPushButton(option,true,":/playscene/res/returnButton.png");
        connect(Return,&MyPushButton::clicked,[=](){
            delete option;
            /***************************************************************************************************/
            //僵尸恢复
            for(int i=0;i<ZombieNumber;++i)
                zombie[i]->Zstart();
            connect(timer,&QTimer::timeout,[=](){
                for(int i=0;i<ZombieNumber;++i)
                {
                    if(zombie[i]->hp<=0)
                    {
                        zombie[i]->Die(zombie[i]->DiePath);
                        if(zombie[i]->movie->frameCount()==zombie[i]->movie->currentFrameNumber()+1)
                        {
                            zombie[i]->timer->disconnect();
                            delete zombie[i];
                            int n=i;
                            ++n;
                            for(;n<ZombieNumber;++n)
                                zombie[n-1]=zombie[n];
                            --ZombieNumber;
                            --i;
                        }
                    }
                }
            });
            connect(Ctimer,&QTimer::timeout,[=](){//僵尸随机生成
                Born(FCzombie[Zcnt%10],FCraw[Zcnt%10]);
                ++Zcnt;
            });
            /***************************************************************************************************/
        });
        Return->move((option->width()-Return->width())*0.5,540);
        //主菜单按钮
        MyPushButton* menu=new MyPushButton(option,true,":/playscene/res/mainMenu.png");
        connect(menu,&MyPushButton::clicked,[=](){
            /***************************************************************************************************/
            //返回主菜单,timer全disconnect
            for(int i=0;i<ZombieNumber;++i)
            {
                zombie[i]->timer->disconnect();
            }
            ZombieNumber=0;
            timer->disconnect();
            Ctimer->disconnect();
            /***************************************************************************************************/
            battelbgm->stop();
            emit mainmenu();
        });
        menu->move((option->width()-menu->width())*0.5,450);
        option->show();

    });

 //植物部分
    memset(map,0,sizeof(map));

    xlimit[0]=53;
    xlimit[1]=153;
    xlimit[2]=253;
    xlimit[3]=353;
    xlimit[4]=453;
    xlimit[5]=553;
    xlimit[6]=653;
    xlimit[7]=753;
    xlimit[8]=853;
    xlimit[9]=953;

    ylimit[0]=100;
    ylimit[1]=210;
    ylimit[2]=320;
    ylimit[3]=430;
    ylimit[4]=540;
    ylimit[5]=650;


}


void playscene::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QImage bg(":/playscene/res/Background.jpg");
    bg=bg.scaled(size(),Qt::KeepAspectRatioByExpanding);
    painter.drawImage(-250,0,bg);
}
void playscene::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<event->x()<<' '<<event->y();
    if(isPlanting!=-1)
    {
        if(event->button()==Qt::RightButton)
        {
            setCursor(Qt::ArrowCursor);
            if(isPlanting>=0)
                seedBank[isPlanting]->checksun(sunnum);
            else
                shovel->setIcon(shovel->picture1);
            isPlanting=-1;
            return;
        }
        else if(QRect(60,100,860,580).contains(event->pos()))
        {
            if(isPlanting>=0)
            {

                if(visited(event->x(),event->y())){
                    createplant(isPlanting);//此处和下一行目前可用于调整植物种类
                    born(isPlanting);//方法是调整这两个函数的第一个参数（1-5）
                }

                setCursor(Qt::ArrowCursor);
                sunnum-=seedBank[isPlanting]->sun;
                sunlabel->setText(QString::number(sunnum));
                seedBank[isPlanting]->cdstart();

                for(int i=0;i<5;i++)
                {
                    if(!seedBank[i]->incd)
                        seedBank[i]->checksun(sunnum);
                }
                QSound::play(":/playscene/res/plant.wav");
                isPlanting=-1;
                return;
            }
            else
            {
                setCursor(Qt::ArrowCursor);
                shovel->setIcon(shovel->picture1);
                QSound::play(":/playscene/res/plant.wav");
                isPlanting=-1;
                return;
            }
        }
    }
    return QWidget::mousePressEvent(event);
}




/***************************************************************************************************/
//僵尸诞生函数
void playscene::Born(int Number,int raw)
{
    if(ZombieNumber >= MaxNumber)
        return;
    switch(Number)//1 普通僵尸 2 路障僵尸 3 铁桶僵尸 4 橄榄球僵尸
    {
    case 1:{zombie[ZombieNumber] = new NormalZombie;zombie[ZombieNumber]->setParent(this);zombie[ZombieNumber]->move(900,raw_h[raw]);zombie[ZombieNumber]->Walk(zombie[ZombieNumber]->WalkPath);break;}
    case 2:{zombie[ZombieNumber] = new ConeZombie;zombie[ZombieNumber]->setParent(this);zombie[ZombieNumber]->move(900,raw_h[raw]);zombie[ZombieNumber]->Walk(zombie[ZombieNumber]->WalkPath);break;}
    case 3:{zombie[ZombieNumber] = new BucketZombie;zombie[ZombieNumber]->setParent(this);zombie[ZombieNumber]->move(900,raw_h[raw]);zombie[ZombieNumber]->Walk(zombie[ZombieNumber]->WalkPath);break;}
    case 4:{zombie[ZombieNumber] = new FootballZombie;zombie[ZombieNumber]->setParent(this);zombie[ZombieNumber]->move(900,raw_h[raw]);zombie[ZombieNumber]->Walk(zombie[ZombieNumber]->WalkPath);break;}
    default:{return;}
    }
    zombie[ZombieNumber]->resize(400,160);
    zombie[ZombieNumber]->Znumber=Number;
    zombie[ZombieNumber]->Change();
    ++ZombieNumber;
}
/***************************************************************************************************/

//以下为植物部分

int playscene::xtrans()
{
    int px;
    if(clix==1)px=70;
    if(clix==2)px=225;
    if(clix==3)px=280;
    if(clix==4)px=335;
    if(clix==5)px=390;
    if(clix==6)px=445;
    if(clix==7)px=500;
    if(clix==8)px=555;
    if(clix==9)px=610;
    return px;
}

int playscene::ytrans()
{
    int py;
    if(cliy==1)py=50;
    if(cliy==2)py=230;
    if(cliy==3)py=350;
    if(cliy==4)py=470;
    if(cliy==5)py=590;
    return py;
}

bool playscene::visited(int x,int y)
{//判断鼠标点击的方格是否已经有植物
    int i=0,j=0,f1=0,f2=0,xx,yy;
    while(i<=8){
        if(x>=xlimit[i]&&x<=xlimit[i+1]){
            xx=i+1;
            f1=1;
        }
        i++;
    }
    while(j<=4){
        if(y>=ylimit[j]&&y<=ylimit[j+1]){
            yy=j+1;
            f2=1;
        }
        j++;
    }
    if(f1==1&&f2==1&&map[xx][yy]==0){
        clix=xx;
        cliy=yy;
        return true;
    }
    return false;
}

void playscene::createplant(int planttype)//功能是创建植物的gif图形
{//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手
    int px,py;
    px=xtrans();
    py=ytrans();
    if(planttype!=3){
    QLabel *label_movie=new QLabel(this);
    label_movie=new QLabel(this);
    label_movie->resize(60,60);//标签设置大小
    label_movie->move(px,py);//标签设置位置
    QMovie *movie;
    if(planttype==1){
        movie=new QMovie(":/pltimg/plantimages/SunFlower.gif");//动图的资源
    }
    if(planttype==2){
        movie=new QMovie(":/pltimg/plantimages/Peashooter.gif");
    }
    if(planttype==4){
        movie=new QMovie(":/pltimg/plantimages/PotatoMine1.gif");
    }
    if(planttype==5){
        movie=new QMovie(":/pltimg/plantimages/Repeater.gif");
    }
    label_movie->setMovie(movie);//设置标签的动图
    label_movie->setScaledContents(true);//设置为自适应大小
    movie->start();
    label_movie->show();
    pic[clix][cliy]=label_movie;
}
}

void playscene::born(int planttype)
{//功能为实现各植物的特性
    map[clix][cliy]=planttype;
    if(planttype==1){//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手
        plthp[clix][cliy]=100;
        int xx=xtrans(),yy=ytrans(),cx=clix,cy=cliy;
        QTimer * timer=new QTimer(this);
        timer->start(2000);
        connect(timer,&QTimer::timeout,[=](){
            QLabel *label_movie=new QLabel(this);
            label_movie=new QLabel(this);
            label_movie->resize(60,60);
            label_movie->move(xx+10,yy+15);
            QMovie *movie=new QMovie(":/pltimg/plantimages/Sun.gif");
            label_movie->setMovie(movie);
            label_movie->setScaledContents(true);
            movie->start();
            label_movie->show();
            if(plthp[cx][cy]<=0){
                timer->stop();
                plthp[cx][cy]=0;
                map[cx][cy]=0;
                pic[cx][cy]->clear();
            }
        });
    }
    if(planttype==2){//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手
        plthp[clix][cliy]=100;
        int xx=xtrans(),yy=ytrans(),cx=clix,cy=cliy;
        QTimer * timer0=new QTimer(this);
        timer0->start(2000);
        connect(timer0,&QTimer::timeout,[=](){
        QTimer * timer=new QTimer(this);
        timer->start(33);
        QLabel *label_movie=new QLabel(this);
        label_movie=new QLabel(this);
        label_movie->resize(25,25);
        label_movie->move(xx+40,yy+2);
        QMovie *movie=new QMovie(":/pltimg/plantimages/Pea.png");
        label_movie->setMovie(movie);
        label_movie->setScaledContents(true);
        movie->start();
        label_movie->show();
        connect(timer,&QTimer::timeout,[=](){
            label_movie->move(label_movie->pos().x()+10,label_movie->pos().y());
            label_movie->show();
            if(label_movie->pos().x()>1000){
                timer->stop();
                label_movie->clear();
            }
      });
        if(plthp[cx][cy]<=0){
            timer0->stop();
            plthp[cx][cy]=0;
            map[cx][cy]=0;
            pic[cx][cy]->clear();
        }
    });
    }
    if(planttype==3){//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手

        int px,py;
        px=xtrans();
        py=ytrans();
        QLabel *label_movie=new QLabel(this);
        label_movie=new QLabel(this);
        label_movie->resize(60,60);
        label_movie->move(px,py);
        QMovie *movie;
        movie=new QMovie(":/pltimg/plantimages/WallNut.gif");
        label_movie->setMovie(movie);
        label_movie->setScaledContents(true);
        movie->start();
        label_movie->show();

        plthp[clix][cliy]=1000;

        int cx=clix,cy=cliy,xx=xtrans(),yy=ytrans();

        QTimer * timer=new QTimer(this);
        timer->start(1);

        connect(timer,&QTimer::timeout,[=](){

            if(plthp[cx][cy]>=300&&plthp[cx][cy]<=700){
                label_movie->close();
                QLabel *label_movie2=new QLabel(this);
                label_movie2=new QLabel(this);
                label_movie2->resize(60,60);
                label_movie2->move(xx,yy);
                QMovie *movie2=new QMovie(":/pltimg/plantimages/WallNut1.gif");
                label_movie2->setMovie(movie2);
                label_movie2->setScaledContents(true);
                movie2->start();
                label_movie2->show();

                timer->stop();

                QTimer * timer2=new QTimer(this);
                timer2->start(1);
                 connect(timer2,&QTimer::timeout,[=](){

                     if(plthp[cx][cy]<300){
                         timer2->stop();
                         label_movie2->close();
                         QLabel *label_movie3=new QLabel(this);
                         label_movie3=new QLabel(this);
                         label_movie3->resize(60,60);
                         label_movie3->move(xx,yy);
                         QMovie *movie3=new QMovie(":/pltimg/plantimages/WallNut2.gif");
                         label_movie3->setMovie(movie3);
                         label_movie3->setScaledContents(true);
                         movie3->start();
                         label_movie3->show();

                         QTimer * timer3=new QTimer(this);
                         timer3->start(1);
                          connect(timer3,&QTimer::timeout,[=](){
                              if(plthp[cx][cy]<=0){
                                  timer3->stop();
                                  plthp[cx][cy]=0;
                                  map[cx][cy]=0;
                                  label_movie3->clear();
                              }
                          });

                     }

                 });
            }

       });
    }
    if(planttype==4){//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手
        plthp[clix][cliy]=100;
        int xx=xtrans(),yy=ytrans(),cx=clix,cy=cliy;
        QTimer::singleShot(1000,[=](){
            pic[cx][cy]->clear();
            QLabel *label_movie=new QLabel(this);
            label_movie=new QLabel(this);
            label_movie->resize(60,60);
            label_movie->move(xx,yy);
            QMovie *movie=new QMovie(":/pltimg/plantimages/PotatoMine.gif");
            label_movie->setMovie(movie);
            label_movie->setScaledContents(true);
            movie->start();
            label_movie->show();
            plthp[cx][cy]=5000;
            QTimer * timer2=new QTimer(this);
            timer2->start(1);
             connect(timer2,&QTimer::timeout,[=](){
            if(plthp[cx][cy]<=0){
                timer2->stop();
                plthp[cx][cy]=0;
                map[cx][cy]=0;
                label_movie->clear();
            }
             });
        });
    }
    if(planttype==5){//plant type：1：向日葵 2：豌豆射手 3：坚果 4：土豆地雷 5：双发射手
        plthp[clix][cliy]=100;
        int xx=xtrans(),yy=ytrans(),cx=clix,cy=cliy;
        QTimer * timer0=new QTimer(this);
        timer0->start(2000);
        connect(timer0,&QTimer::timeout,[=](){
        QTimer * timer=new QTimer(this);
        timer->start(33);
        QLabel *label_movie=new QLabel(this);
        label_movie=new QLabel(this);
        label_movie->resize(25,25);
        label_movie->move(xx+40,yy+2);
        QMovie *movie=new QMovie(":/pltimg/plantimages/Pea.png");
        label_movie->setMovie(movie);
        label_movie->setScaledContents(true);
        movie->start();
        label_movie->show();
        connect(timer,&QTimer::timeout,[=](){
            label_movie->move(label_movie->pos().x()+10,label_movie->pos().y());
            label_movie->show();
            if(label_movie->pos().x()>1000){
                timer->stop();
                label_movie->clear();
            }
      });
        QTimer::singleShot(300,[=](){
            QTimer * timer=new QTimer(this);
            timer->start(33);
            QLabel *label_movie=new QLabel(this);
            label_movie=new QLabel(this);
            label_movie->resize(25,25);
            label_movie->move(xx+40,yy+2);
            QMovie *movie=new QMovie(":/pltimg/plantimages/Pea.png");
            label_movie->setMovie(movie);
            label_movie->setScaledContents(true);
            movie->start();
            label_movie->show();
            connect(timer,&QTimer::timeout,[=](){
                label_movie->move(label_movie->pos().x()+10,label_movie->pos().y());
                label_movie->show();
                if(label_movie->pos().x()>1000){
                    timer->stop();
                    label_movie->clear();
                }
          });
        });
        if(plthp[cx][cy]<=0){
            timer0->stop();
            plthp[cx][cy]=0;
            map[cx][cy]=0;
            pic[cx][cy]->clear();
        }
    });
    }
}


