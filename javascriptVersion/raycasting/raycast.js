const TILE_SIZE = 32;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIPE_WIDTH = 4;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIPE_WIDTH;


class Map {
    constructor() {
        this.grid = [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        ];
    }
    hasWallAt(x,y) {
        if(x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
            return true;
        }
        var mapGridIndexX = Math.floor(x / TILE_SIZE);
        var mapGridIndexY = Math.floor(y / TILE_SIZE);
        return this.grid[mapGridIndexY][mapGridIndexX] != 0;
    }
    render() {
        for (var i = 0; i < MAP_NUM_ROWS; i++) {
            for (var j = 0; j < MAP_NUM_COLS; j++) {
                var tileX = j * TILE_SIZE; 
                var tileY = i * TILE_SIZE;
                var tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

class Player {
    constructor() {
        this.x = WINDOW_WIDTH / 2;
        this.y = WINDOW_HEIGHT / 2;
        this.radius = 3;
        this.turnDirection = 0;
        this.walkDirection = 0;
        this.rotationAngle = Math.PI/2;
        this.moveSpeed = 1;
        this.rotationSpeed = 2 * (Math.PI / 180);
    }
    render() {
        fill("yellow");
        circle(this.x, this.y, this.radius);
        stroke('magenta');
        strokeWeight(1);
        line(this.x, this.y, this.x + Math.cos(this.rotationAngle) * 20,this.y + Math.sin(this.rotationAngle) * 20);
    }
    update() {
        this.rotationAngle += this.turnDirection * this.rotationSpeed;
        //y wall collision
        var xStep = Math.cos(this.rotationAngle) * (this.moveSpeed * this.walkDirection);
        var yStep = Math.sin(this.rotationAngle) * (this.moveSpeed * this.walkDirection);
        if (this.walkDirection != 0) {
            this.y += yStep;
            if (grid.hasWallAt(this.x, this.y) == 1) {
                this.y -= yStep;
            }
            //check collision -> convert position to grid.
            this.x += xStep;
            if (grid.hasWallAt(this.x, this.y) == 1) {
                this.x -= xStep;
            }
        }

    }
}
function normalizeAngle(angle) {
    
    angle = angle % (2 * Math.PI);
    if(angle < 0) {
        angle += 2 * Math.PI;
    }
    return angle;
}
function distance(x1,y1,x2,y2) {
    return Math.sqrt(((Math.abs(x2 - x1))^2) + ((Math.abs(y2 - y1))^2));
}
class Ray {
    constructor(rayAngle) {
        this.rayAngle = normalizeAngle(rayAngle);
        //console.log(this.rayAngle);
        this.xDistance = 0;
        this.yDistance = 0;
        this.wallHitXHori = 0;
        this.wallHitYHori = 0;
        this.wallHitXVert = 0;
        this.wallHitYVert = 0;
        this.distanceHori = 0;
        this.distanceVert = 0;

        this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
        this.isRayFacingUp = !this.isRayFacingDown;
        this.isRayFacingRight = this.rayAngle > Math.PI * 1.5 || this.rayAngle < Math.PI * 0.5;
        this.isRayFacingLeft = !this.isRayFacingRight;

    }
    cast(columnId) {
        //Horizontal Ray-Grid Interesection Code
        var xinterceptHori, yinterceptHori;
        var xstep, ystep;
        
        yinterceptHori = Math.floor(player.y/TILE_SIZE) * TILE_SIZE;
        yinterceptHori += this.isRayFacingDown ? TILE_SIZE : 0;
        xinterceptHori = player.x + ((yinterceptHori - player.y)/Math.tan(this.rayAngle.toFixed(6)));

        ystep = TILE_SIZE;
        ystep *= this.isRayFacingUp ? -1 : 1;
        xstep = TILE_SIZE / Math.tan(this.rayAngle);
        xstep *= (this.isRayFacingLeft && xstep > 0) ? -1 : 1;
        xstep *= (this.isRayFacingRight && xstep < 0) ? -1 : 1;
        var xz = 0;
        if(this.isRayFacingUp) {
            yinterceptHori--;
        }
        while (xz < 10) {
            this.wallHitXHori = xinterceptHori;
            this.wallHitYHori = yinterceptHori;
            //fix for edge case where I wasnt detecting a wall.
            if (grid.hasWallAt(this.wallHitXHori,this.wallHitYHori)) {
                xz = 10;
                if(this.isRayFacingUp){
                    this.wallHitYHori = this.wallHitYHori+1;
                }
                this.distanceHori = distance(player.x, player.y, this.wallHitXHori, this.wallHitYHori);
            }
            xinterceptHori += xstep;
            yinterceptHori += ystep;
            xz++;
        }

        //Vertical Ray-Grid Interesection Code
        var xinterceptVert, yinterceptVert;
        var xstepVert, ystepVert;
        
        xinterceptVert = Math.floor(player.x/TILE_SIZE) * TILE_SIZE;
        xinterceptVert += this.isRayFacingRight ? TILE_SIZE : 0;
        yinterceptVert = player.y + ((xinterceptVert - player.x) * Math.tan(this.rayAngle.toFixed(6)));
        xstepVert = TILE_SIZE;
        xstepVert *= this.isRayFacingLeft ? -1 : 1;
        ystepVert = TILE_SIZE * Math.tan(this.rayAngle);
        ystepVert *= (this.isRayFacingUp && ystepVert > 0) ? -1 : 1;
        ystepVert *= (this.isRayFacingDown && ystepVert < 0) ? -1 : 1;
        var xzy = 0;
        if(this.isRayFacingLeft) {
            xinterceptVert--;
        }
        while (xzy < 10) {

            this.wallHitXVert = xinterceptVert;
            this.wallHitYVert = yinterceptVert;
            //fix for edge case where I wasnt detecting a wall.
            if (grid.hasWallAt(this.wallHitXVert,this.wallHitYVert)) {
                xzy = 10;
                if(this.isRayFacingLeft) {
                    this.wallHitXVert++;
                }
                this.distanceVert = distance(player.x, player.y, this.wallHitXVert, this.wallHitYVert);
            }
            xinterceptVert += xstepVert;
            yinterceptVert += ystepVert;
            xzy++;
        }

      //  compare sizes
       if(Math.abs(this.distanceHori) < Math.abs(this.distanceVert)) {
           this.wallHitXVert = this.wallHitXHori;
           this.wallHitYVert = this.wallHitYHori;
       }
    }
    render () {
        stroke("rgba(255,0,0,0.3)");
        line(player.x, player.y, this.wallHitXVert, this.wallHitYVert);
    }
}
var grid = new Map();
var player = new Player();
var rays = [];
function keyPressed() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 1;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = -1;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 1;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = -1;
    }
}
function keyReleased() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 0;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = 0;
    }
}
function setup() {
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function castAllRays() {
    var columnId = 0;
    //start first ray subtracting half of my FOV
    var rayAngle = player.rotationAngle - (FOV_ANGLE/2);

    rays = [];

    //loop all columns casting the rays.
    for (var i = 0; i < NUM_RAYS; i++) {
        var ray = new Ray(rayAngle);
        ray.cast(columnId);
        rays.push(ray);
        rayAngle += FOV_ANGLE / NUM_RAYS;
        columnId ++;
    }
}
function update() {
    // TODO: update all game objects before we render the next frame
    player.update()
    castAllRays();
}

function draw() {
    update();

    grid.render();
    
    for(ray of rays) {
        ray.render();
    }
    player.render();
}
