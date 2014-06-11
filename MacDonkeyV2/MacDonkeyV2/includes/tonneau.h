//
//  tonneau.h
//  TestSDL
//
//  Created by Jo' on 27/05/2014.
//  Copyright (c) 2014 azer. All rights reserved.
//

#ifndef TestSDL_tonneau_h
#define TestSDL_tonneau_h

typedef struct  Tonneau{ // donnele nom à la structure
   
    ///position
    float x;
    float y;
    /// Vitesse
    float speedX;
    float speedY;
    /// taille
    float width;
    float height;

} Tonneau; //type de la structure

Tonneau createTonneau(float x, float y, float speedX, float speedY, float width, float height);

#endif
