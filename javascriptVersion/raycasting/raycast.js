const TILE_SIZE = 32;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIPE_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIPE_WIDTH;
const PROJECTION_DISTANCE = (WINDOW_WIDTH/2)/Math.tan(FOV_ANGLE / 2);
const MINIMAP_SCALE_FACTOR = 0.3;
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
                rect(MINIMAP_SCALE_FACTOR * tileX, MINIMAP_SCALE_FACTOR * tileY, MINIMAP_SCALE_FACTOR * TILE_SIZE, MINIMAP_SCALE_FACTOR * TILE_SIZE);
            }
        }
    }
}
class Wall {
    constructor(distance, wallHeight) {
        this.distance = distance;
        this.height = wallHeight;
        this.width = WALL_STRIPE_WIDTH; //columnId * wallstripewidth
        this.x = WINDOW_WIDTH; //should be ray columnId
        this.y = 0; //starting point of the wall. Wall should be centered.
    }
    create(columnId) {
        this.x = (columnId * WALL_STRIPE_WIDTH);
        this.y = (WINDOW_HEIGHT/2) - (this.height/2); //should be dynamic or i can make it dynamic.
    }
    render() {
        stroke("rgba(255, 0, 0, 1.0)");
        fill("rgba(255, 0, 0, 1.0)")
        //console.log(this.y);
        rect(this.x, this.y, this.width, this.height);
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
        noStroke();
        fill("yellow");
        circle(MINIMAP_SCALE_FACTOR * this.x, MINIMAP_SCALE_FACTOR * this.y, MINIMAP_SCALE_FACTOR * this.radius);
        stroke('magenta');
        strokeWeight(1);
        line(MINIMAP_SCALE_FACTOR * this.x, MINIMAP_SCALE_FACTOR * this.y, MINIMAP_SCALE_FACTOR * (this.x + Math.cos(this.rotationAngle) * 20),MINIMAP_SCALE_FACTOR * (this.y + Math.sin(this.rotationAngle) * 20));
    }
    update() {
        this.rotationAngle += this.turnDirection * this.rotationSpeed;
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

    return Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
class Ray {
    constructor(rayAngle) {
        this.rayAngle = normalizeAngle(rayAngle);
        this.wallHitX = 0;
        this.wallHitY = 0;
        this.wallHitXHori = 0;
        this.wallHitYHori = 0;
        this.wallHitXVert = 0;
        this.wallHitYVert = 0;
        this.distanceHori = 0;
        this.distanceVert = 0;
        this.distance = 0;
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
        xinterceptHori = player.x + (yinterceptHori - player.y) / Math.tan(this.rayAngle);

        ystep = TILE_SIZE;
        ystep *= this.isRayFacingUp ? -1 : 1;

        xstep = TILE_SIZE / Math.tan(this.rayAngle);
        xstep *= (this.isRayFacingLeft && xstep > 0) ? -1 : 1;
        xstep *= (this.isRayFacingRight && xstep < 0) ? -1 : 1;
        var xz = 0;

        while (xz < 16) {
            this.wallHitXHori = xinterceptHori;
            this.wallHitYHori = yinterceptHori;
            if (grid.hasWallAt(this.wallHitXHori,this.wallHitYHori - (this.isRayFacingUp ? 1 : 0))) {
                xz = 16;
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

        yinterceptVert = player.y + (xinterceptVert - player.x) * Math.tan(this.rayAngle);

        xstepVert = TILE_SIZE;
        xstepVert *= this.isRayFacingLeft ? -1 : 1;

        ystepVert = TILE_SIZE * Math.tan(this.rayAngle);
        ystepVert *= (this.isRayFacingUp && ystepVert > 0) ? -1 : 1;
        ystepVert *= (this.isRayFacingDown && ystepVert < 0) ? -1 : 1;
        var xzy = 0;
        while (xzy < 16) {

            this.wallHitXVert = xinterceptVert;
            this.wallHitYVert = yinterceptVert;
            if (grid.hasWallAt(this.wallHitXVert - (this.isRayFacingLeft ? 1 : 0),this.wallHitYVert)) {
                xzy = 16;
                this.distanceVert = distance(player.x, player.y, this.wallHitXVert, this.wallHitYVert);
            }
            xinterceptVert += xstepVert;
            yinterceptVert += ystepVert;
            xzy++;
        }

       if (Math.abs(this.distanceHori) < Math.abs(this.distanceVert)) {
        this.distance = this.distanceHori;
        this.wallHitX = this.wallHitXHori;
        this.wallHitY = this.wallHitYHori;
       }
       else {
        this.distance = this.distanceVert;
        this.wallHitX = this.wallHitXVert;
        this.wallHitY = this.wallHitYVert;
       }
       this.distance = Math.cos(this.rayAngle - player.rotationAngle) * this.distance;
    }
    render () {
        stroke("rgba(255, 0, 0, 1.0)");
        line(MINIMAP_SCALE_FACTOR * player.x, MINIMAP_SCALE_FACTOR * player.y, MINIMAP_SCALE_FACTOR * this.wallHitX, MINIMAP_SCALE_FACTOR * this.wallHitY);
    }
}
var grid = new Map();
var player = new Player();
var rays = [];
var walls = [];
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
    walls = [];
    //loop all columns casting the rays.
    for (var i = 0; i < NUM_RAYS; i++) {
        var ray = new Ray(rayAngle);
        ray.cast(columnId);
        rays.push(ray);
        rayAngle += FOV_ANGLE / NUM_RAYS;
        
        //generate wall based on ray:

        //calculate distance from projection plane:
        var wallHeight = TILE_SIZE/2;
        var playerDist = ray.distance;
        
        var projectedWallHeight = (wallHeight/playerDist * PROJECTION_DISTANCE) * 2;
        var wall = new Wall(PROJECTION_DISTANCE, projectedWallHeight);
        wall.create(columnId);

        walls.push(wall);
        columnId ++;
    }
}
function update() {
    // TODO: update all game objects before we render the next frame
    player.update();
    castAllRays();
    
    
}

function draw() {

    clear("#212121");
    update();

    for (wall of walls) {
        
        wall.render();
    }

    
    grid.render();
    player.render();
    for(ray of rays) {
        ray.render();
    }
}
