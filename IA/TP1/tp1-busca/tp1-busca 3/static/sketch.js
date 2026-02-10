let w = 0;
let h = 0;
let s = 0;
let cost = 1;

let isSPressed = false;
let isGPressed = false;
let isCPressed = false;

let init = null;
let end = null;

let path = [];
let visited = [];
let maps = [];
let world = [];

function getMaps()  {
    // Construct the URL with the message as a query parameter
    var url = 'http://localhost:5001/get_maps';

    // Make a GET request to the server
    fetch(url)
    .then(response => response.json())
    .then(data => {
        // Process the response from the server
        loadMaps(data.maps);
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

function startSearch() {
    if (!validateMap()) {
        return;
    }
    
    let map = worldToMap();
    console.log(map);
    let alg = document.getElementById('search').value;
    let heuristic = document.getElementById('heuristic').value;

    // Construct the URL with parameters
    var url = `http://localhost:5001/start_search?map=${encodeURIComponent(map)}&alg=${encodeURIComponent(alg)}&heuristic=${encodeURIComponent(heuristic)}`;

    // Make a GET request to the server
    fetch(url)
    .then(response => response.json())
    .then(data => {
        console.log(data);
        // Process the response from the server
        path = data.path;
        visited = data.visited;
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

function startMap() {
    init = null;
    end = null;
    path = [];
    visited = []

    let mapW = Math.floor(w/s);
    let mapH = Math.floor(h/s);

    world = new Array(mapW);
    for (let i = 0; i < mapW; i++) {
        world[i] = new Array(mapH);
        for (let j = 0; j < mapH; j++) {
            world[i][j] = '1';
        }
    }     
}

function loadMaps(data)  {
    let select = document.getElementById('maps');

    // Clear select
    select.innerHTML = '';
    select.options.length = 0;
    select.options[select.options.length] = new Option('Novo mapa', '');

    let mapIndex = 1;
    for (var key in data) {
        if (data.hasOwnProperty(key)) {
          let option = document.createElement('option');
          option.value = data[key];
          option.text = key;
          select.appendChild(option);

          mapIndex++;
        }
      }

    //   selectedMap = select.value;
    //   loadMap(selectedMap);
}

function loadMap(selectedMap) {
    let mapH = selectedMap.split('\n').length - 1; 
    let mapW = selectedMap.split('\n')[0].length;

    path = [];
    visited = []
    
    world = new Array(mapW);
    for (let i = 0; i < mapW; i++) {
        world[i] = new Array(mapH);
        for (let j = 0; j < mapH; j++) {
            world[i][j] = selectedMap.split('\n')[j][i];

            if (world[i][j] == 'S') {
                init = createVector(i, j);
            }
            else if (world[i][j] == 'G') {
                end = createVector(i, j);
            }
        }
    }

    s = Math.floor(w/mapW) > Math.floor(h/mapH) ? Math.floor(h/mapH) : Math.floor(w/mapW);
    document.getElementById('scaleMap').value = s;
}

function preload() {
    getMaps();
}

function setup() {
    w = 840;
    h = 400;
    s = 25;

    // Create canvas
    let mapCanvas = createCanvas(w, h);
    mapCanvas.parent("mapCanvas");

    // Config inputs
    document.getElementById('scaleMap').value = s;
    document.getElementById('cost').value = cost;
    
    // Init world
    startMap();

    let mapSelect = document.getElementById('maps');
    mapSelect.addEventListener('change', function() {
        let selectedMap = this.value;
        if(selectedMap != '') {
            loadMap(selectedMap);
        }
        else {
            updateMapSize();
        }
    });

    let searchSelect = document.getElementById('search');
    searchSelect.addEventListener('change', function() {
        let selectedSearch = this.value;
        let heuristicSelect = document.getElementById('heuristic');
        if (selectedSearch == 'bfs' || selectedSearch == 'dfs' || selectedSearch == 'ucs') {
            heuristicSelect.disabled = true;x
        }
        else {
            heuristicSelect.disabled = false;
        }
    });
    
}

function updateMapSize() {
    s = document.getElementById('scaleMap').value;
    startMap();
}

function updateCost() {
    cost = document.getElementById('cost').value;
}

function draw() {
    clear();
    
    background(255);
    stroke(240);

    if(world[0] == null){
        return;
    }

    for(let i = 0; i < world.length; i++) {
        for(let j = 0; j < world[0].length; j++) {
            if(world[i][j] == 'X') {
                fill(80); 
            }
            else {
                if(world[i][j] == '1') {
                    fill(255);
                }
                else {
                    let cellValue = parseInt(world[i][j]);
                    let mapColor = map(cellValue, 2, 9, 100, 200);
                    fill(mapColor, 20, 0, mapColor);
                }
            }

            rect(i*s, j*s, s, s);
        }
    }

    // draw visited
    if(visited.length > 0) {
        for(let n of visited) {
            fill(196, 166, 60, 80);
            rect(n[0]*s, n[1]*s, s, s);
        }
    }

    // draw path
    if(path.length > 0) {
        for(let n of path) {
            fill(200);
            rect(n[0]*s, n[1]*s, s, s);
        }
    }

        // draw init
        if(init != null) {
            fill(130, 151, 199);
            rect(init.x*s, init.y*s, s, s);
        }
    
        // draw end
        if(end != null) {
            fill(100, 252, 80);
            rect(end.x*s, end.y*s, s, s);
        }
    
}

function worldToMap() {
    let mapW = Math.floor(w/s);
    let mapH = Math.floor(h/s);

    let map = '';
    for(let i = 0; i < mapH; i++) {
        for(let j = 0; j < mapW; j++) {
            map += world[j][i];
        }
        map += '\n';
    }

    return map;
}

function validateMap() {
    if(init == null) {
        alert('Por favor insira um estado inicial. Segura S e clique no mapa para inserir um estado inicial.');
        return false;
    }

    if(end == null) {
        alert('Por favor insira um estado final. Segura G e clique no mapa para inserir um estado final.');
        return false;
    }

    return true;
}

// =================
// Input Functions
// =================

function saveMap() {
    let mapName = document.getElementById('nameMap').value;

    if(!validateMap()) {
        return false;
    }

    if(mapName == '') {
        alert('Por favor insira um nome para o mapa');
        return false;
    }

    // Construct the URL with parameters
    let map = worldToMap();
    var url = `http://localhost:5001/save_map?map_name=${encodeURIComponent(mapName)}&map=${encodeURIComponent(map)}`;

    // Make a GET request to the server
    fetch(url)
    .then(response => response.json())
    .then(data => {
        // Alert the response from the server
        alert("Mapa salvo com sucesso!");
        getMaps();
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

function mousePressed() {
    let x = int(mouseX/s);
    let y = int(mouseY/s);

    if (mouseButton === LEFT) {
        if(x < 0 || x >= world.length || y < 0 || y >= world[0].length) {
            return;
        }

        if (isSPressed) {
            world[x][y] = 'S';
            if(init != null) {
                world[init.x][init.y] = '1';
            }
            init = createVector(x, y);
            path = [];
            visited = [];
        }
        else if (isGPressed) {
            world[x][y] = 'G';
            if(end != null) {
                world[end.x][end.y] = '1';
            }
            end = createVector(x, y);
            path = [];
            visited = [];
        }
        else {
            if(world[x][y] == 'X') {
                world[x][y] = '1';
            }
            else if(world[x][y] != 'S' && world[x][y] != 'G') {
                if (isCPressed) {
                    world[x][y] = cost;
                }
                else {
                    world[x][y] = 'X';
                }

                path = [];
                visited = [];
            }
        }
    }
}

function mouseDragged() {
    if(mouseButton === LEFT) {
        let x = int(mouseX/s);
        let y = int(mouseY/s);
    
        if(x < 0 || x >= world.length || y < 0 || y >= world[0].length) {
            return;
        }
        
        if (isCPressed) {
            world[x][y] = cost;
        }
        else {
            if (world[x][y] != 'S' && world[x][y] != 'G') {
                world[x][y] = 'X';
                path = [];
                visited = [];
            }
        }
    }
}

function keyPressed() {
    // Check if key s is pressed
    if (keyCode === 83) {
        isSPressed = true;
    }

    // Check if g is pressed
    if (keyCode === 71) {
        isGPressed = true;
    }

    // Check if c is pressed
    if (keyCode === 67) {
        isCPressed = true;
    }
}

function keyReleased() {
    // Check if key s is pressed
    if (keyCode === 83) {
        isSPressed = false;
    }

    // Check if g is pressed
    if (keyCode === 71) {
        isGPressed = false;
    }

    // Check if c is pressed
    if (keyCode === 67) {
        isCPressed = false;
    }
}