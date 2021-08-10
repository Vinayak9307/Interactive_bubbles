#include <SFML/Graphics.hpp>
#include <cmath>

#define PI 3.141592

int SCREEN_W;
int SCREEN_H;

typedef sf::Vector2f vec2;


sf::RenderWindow     app;
sf::Mouse            myMouse;
float                dt = (1.f/60.f);

void draw( );
void drawline( vec2 pos1 , vec2 pos2 ,sf::Color C);
float dist( vec2 a , vec2 b);
void selectBall( void );
void deselectBall( void );
void addForce( void );
void applyPhysics( void );


class Bubble{

    vec2   pos;
    vec2   acc;
    vec2   vel;
    float  radius;
    int    id;
    float  mass;
    sf::CircleShape     *circle;
    bool fill = rand( )%19;

    public :
        Bubble( vec2 _pos ,float _radius ,int _id , vec2 _vel = vec2(0,0),vec2 _acc = vec2(0,0) ){
            this->pos       = _pos;
            this->vel       = _vel;
            this->radius    = _radius;
            this->acc       = _acc;
            this->id        = _id;
            this->mass      = (4/3 * PI * radius * radius * radius );
            this->circle    = new sf::CircleShape( ); 
            this->circle->setPointCount(50);
        }
        void applyforce( vec2 force ){
            vel.x += 200.0 * force.x *dt;
            vel.y += 200.0 * force.y *dt;
        }
        void update( void ){
            acc.x = -0.099999 * vel.x;
            acc.y = -0.099999 * vel.y;
            vel.x += acc.x *dt;
            vel.y += acc.y *dt;
            pos.x += vel.x *dt;
            pos.y += vel.y *dt;

            if( pos .x >= SCREEN_W - radius ){
                pos .x = SCREEN_W - radius;
                vel.x *= -1;
            }
            else if( pos.x <= 0 + radius ){
                pos .x = radius;
                vel.x *= -1;
            }
            else if( pos .y >= SCREEN_H - radius ){
                pos .y = SCREEN_H - radius;
                vel.y *= -1;
            }
            else if( pos.y <= 0 + radius ){
                pos .y = radius;
                vel.y *= -1;
            }

            if( fabs(vel.x * vel.x + vel.y * vel.y) < 0.1f){
                vel = vec2(0,0);
            }

        }
        vec2 getPosition( void ){
            return this->pos;
        }
        float getRadius( void ){
            return this->radius;
        }
        vec2 getVelocity( void ){
            return this->vel;
        }
        float getMass( void ){
            return this->mass;
        }
        int getId( void ){
            return this->id;
        }
        bool checkCollision( Bubble target ){
            vec2 targetPos = target.getPosition( );
            return ( dist(pos,targetPos) < radius + target.getRadius());

        }
        void displace( float overlap , vec2 targetDistance){
            float fdistance = dist(pos , targetDistance);
            this->pos.x -= overlap * (pos.x - targetDistance.x)/fdistance;
            this->pos.y -= overlap * (pos.y - targetDistance.y)/fdistance;
        }
        void setPosition( vec2 point ){
            pos = point;
        }
        void setVelocity( vec2 vel ){
            this->vel = vel;
        }
        bool contain( vec2 point ){
            return ( dist(this->pos,point) <= this->radius );
        }
        void drawCircle( void ){
            float rotationA = atan2f(vel.y , vel.x);
            circle->setRadius( radius );
            if(fill){
                circle->setFillColor( sf::Color::Red );
            }
            else{
            circle->setFillColor( sf::Color::Blue );
            }
            circle->setPosition( pos );
            circle->setOrigin(vec2(radius,radius));
            app.draw( *circle );
            drawline(pos , pos + vec2( radius * cos(rotationA) , radius * sin( rotationA)),sf::Color::White);
        }
};

std::vector<Bubble> vecBubbles;

std::vector<std::pair<Bubble* , Bubble*>> collidingBubbles;

Bubble * isSelected = nullptr;
bool AllowedtoMove = false;

void addBubbles( vec2 pos , float radius);

int main()
{
    srand(time(NULL));
    app.create(sf::VideoMode::getFullscreenModes( )[0],"Led Clock",sf::Style::Fullscreen);
    app.setFramerateLimit(60);

    SCREEN_W = app.getSize( ).x;
    SCREEN_H = app.getSize( ).y;

    sf::Texture tex;
    tex.loadFromFile("D:/Project1/bubbles/resources/window2.png");

    sf::Sprite background;
    background.setTexture(tex);


    for( int i = 0 ; i < 20 ; i++){
    addBubbles( vec2(rand( ) % SCREEN_W , rand( ) % SCREEN_H ),25);
    }
    while(app.isOpen()){

        sf::Event event;
        while(app.pollEvent(event)){

            switch (event.type)
            {
            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    app.close( );
                    break;
                
                default:
                    break;
                }
                break;
            case sf::Event::Closed:
                app.close( );
                break;
            case sf::Event::MouseButtonPressed:
                switch (event.mouseButton.button)
                {
                    case sf::Mouse::Left:
                        AllowedtoMove = true;
                        selectBall( );
                        break;
                    case sf::Mouse::Right:
                        selectBall( );
                        break;
                    default :
                        break;
                }
                break;
            case sf::Event::MouseButtonReleased:
                switch(event.mouseButton.button)
                {
                    case sf::Mouse::Right:
                        addForce( );
                        deselectBall( );
                        break;
                    case sf::Mouse::Left:
                        deselectBall( );
                        break;
                    default :
                        break;
                }
                break;
            
            default:
                break;
            }

        }
        app.clear( );
        app.draw( background );
        draw( );
        app.display( );
    }
}
void draw( ){
    for(auto &c:vecBubbles)
    {
        c.update( );
        c.drawCircle( );
    }

    if( isSelected != nullptr ){
        if(AllowedtoMove){
        isSelected->setPosition(app.mapPixelToCoords(sf::Mouse::getPosition(app)));
        isSelected->setVelocity( vec2(0,0));
        }
        else
        drawline( isSelected->getPosition( ) , app.mapPixelToCoords( sf::Mouse::getPosition( app )),sf::Color::Blue);
    }
    
    for(auto &ball:vecBubbles){
        for(auto &target:vecBubbles){
            if(ball.getId( ) != target.getId( )){
                if(ball.checkCollision( target )){
                    float fdistance = dist( ball.getPosition( ) , target.getPosition( ));
                    float overlap = 0.5f * (fdistance - ball.getRadius( ) - target.getRadius( ));

                    ball.displace( overlap , target.getPosition( ));
                    target.displace( overlap , ball.getPosition( ));

                    collidingBubbles.push_back({&ball,&target});
                }
            }
        }
    }

    for(auto c : collidingBubbles){
        drawline( c.first->getPosition( ) , c.second->getPosition( ),sf::Color::Red);
    }
    applyPhysics( );

    collidingBubbles.clear( );
}
void addBubbles( vec2 pos , float radius){

    Bubble * temp = new Bubble( pos , radius , vecBubbles.size( ));
    vecBubbles.push_back( *temp );
}


void drawline( vec2 pos1 , vec2 pos2,sf::Color c){
    sf::Vertex line[] = {
        sf::Vertex(pos1,c),
        sf::Vertex(pos2,c)
    };
    app.draw(line,2,sf::Lines);
}
float dist( vec2 a , vec2 b){
    return sqrt(( a.x - b.x )*( a.x - b.x ) + ( a.y - b.y )*( a.y - b.y ));
}
void selectBall( void ){
        isSelected = nullptr;
       for( unsigned i = 0 ; i < vecBubbles.size( );i++){
           if(vecBubbles[i].contain(app.mapPixelToCoords(sf::Mouse::getPosition( app )))){
           isSelected = &vecBubbles[i];
           break;
           }
       }
}
void deselectBall( void ){
   isSelected = nullptr;
   AllowedtoMove = false;
}
void addForce( void ){
    if( isSelected != nullptr ){
    vec2 distance = (isSelected->getPosition( ) - app.mapPixelToCoords(sf::Mouse::getPosition(app)));
    isSelected->applyforce(distance);
    }
}
void applyPhysics( void ){
    for( auto c : collidingBubbles){
        Bubble *a = c.first;
        Bubble *b = c.second;

        float fdistance = dist( a->getPosition( ), b->getPosition( ));

        float nx = ( a->getPosition( ).x - b->getPosition( ).x)/fdistance;
        float ny = ( a->getPosition( ).y - b->getPosition( ).y)/fdistance;

        float tx = -ny;
        float ty = nx;

        //Dot product of tangent

        float dtan1 = a->getVelocity( ).x * tx + a->getVelocity( ).y * ty;
        float dtan2 = b->getVelocity( ).x * tx + b->getVelocity( ).y * ty;

        //Dot product of normal

        float dnormal1 = a->getVelocity( ).x * nx + a->getVelocity( ).y * ny;
        float dnormal2 = b->getVelocity( ).x * nx + b->getVelocity( ).y * ny;

        float n1 = (dnormal1 * ( a->getMass( ) - b->getMass( )) + 2.0f * b->getMass( ) * dnormal2) / (a->getMass( ) + b->getMass( ));
        float n2 = (dnormal2 * ( b->getMass( ) - a->getMass( )) + 2.0f * a->getMass( ) * dnormal1) / (a->getMass( ) + b->getMass( ));

        a->setVelocity( vec2( dtan1 * tx + n1 * nx , dtan1 * ty + n1 * ny));
        b->setVelocity( vec2( dtan2 * tx + n2 * nx , dtan2 * ty + n2 * ny));
    }
}