var w = 840;
var h = 460;

var nn = null;
var hidden_size = 8;

var display = "tiles";
var allowDraw = true;

// Connect to WebSocket server
const socket = io('http://localhost:5001');

// Event listener for when the connection is established
socket.on('connect', function () {
    console.log('Connected to WebSocket server');
});

// Event listener for incoming messages
socket.on('message_from_server', function (message) {    
    if (message['type'] == "step") {
        // Enable save button
        document.getElementById("saveResults").disabled = false;

        let epoch = message['epoch'];
        let loss = message['loss'];
        let accuracy = message['accuracy'];
        let parameters = message['parameters'];
        
        loadWeightsFromServer(parameters);
        
        document.getElementById("epoch").innerHTML = epoch;
        document.getElementById("loss").innerHTML = loss;
        document.getElementById("accuracy").innerHTML = accuracy;

        socket.emit('message_from_client',  {"type": "step"});
    }
    else if (message['type'] == "finish") {
        // Enable start button
        document.getElementById("startSearch").disabled = false;
    }
});

function startSearch() { 

  // Disable buttons
  document.getElementById("startSearch").disabled = true;

  // Get parameters
  let maxEpochs = document.getElementById("epochsMax").value;
  let learningRate = document.getElementById("learningRate").value;
  let hiddenSize = document.getElementById("hiddenSize").value;
  let init_parameters = document.getElementById("init_parameters").checked;

  // Construct the URL with parameters
  var url = `http://localhost:5001/start_learn?max_epochs=${maxEpochs}&learning_rate=${learningRate}&hidden_size=${hiddenSize}&init_parameters=${init_parameters}`;

  // Make a GET request to the server
  fetch(url)
  .then(response => response.json())
  .then(data => {
      // Process the response from the server
      socket.emit('message_from_client',  {"type": "step"});
  })
  .catch(error => {
      // Enable start button
      document.getElementById("startSearch").disabled = false;

      console.error('Error:', error);
  });

}

function setup() {
    // Create canvas
    let mapCanvas = createCanvas(w, h);
    mapCanvas.parent("mapCanvas");

    // Init hiperparameters
    document.getElementById("epochsMax").value = 10000;
    document.getElementById("learningRate").value = 0.01;
    document.getElementById("hiddenSize").value = hidden_size;
    document.getElementById("init_parameters").checked = true;

    // Create notepad
    notepad = new Notepad(100, 130, 7, 28, 28);

    // Create neural network
    nn = new NeuralNetwork([784, hidden_size, 10]);
    nn.initVisualization(380, 30, 240, 400, false)

    // Create output elements
    marker = new Marker();
    outputDisplay = new OutputDisplay(width - 190, nn.y + nn.h / 2 - 230 / 2, 85, 230);

    document.getElementById("hiddenSize").addEventListener("input", function(e) {
        console.log(e.target.value);
        nn = new NeuralNetwork([784, e.target.value, 10]);
        nn.initVisualization(380, 30, 240, 400, false);
    });
}

function draw() {    
    background(0);
    stroke(240);

    notepad.update(mouseX, mouseY);
    notepad.display();

    nn.display(255 * 50);
    guess();
    marker.update();

    if (!notepad.empty) {
      marker.display();
    }
      
    outputDisplay.display();
}

function guess() {
  currentGuess = nn.guess(normalizeArray(notepad.cells, 255));
}

function loadWeightsFromServer(parameters) {
  W1 = Matrix.transpose(Matrix.fromArray2D(parameters[0]));
  b1 = Matrix.transpose(Matrix.fromArray2D(parameters[1]));
  W2 = Matrix.transpose(Matrix.fromArray2D(parameters[2]));
  b2 = Matrix.transpose(Matrix.fromArray2D(parameters[3]));

  nn.setWeightsFromMatrix([W1, W2]);
  nn.setBiasesFromMatrix([b1, b2]);
  nn.initVisualization(380, 30, 240, 400, false)
}

function saveResults() {
    // Construct the URL with parameters
    var url = `http://localhost:5001/save_results`;

    // Make a GET request to the server
    fetch(url)
    .then(response => response.json())
    .then(data => {
        // Process the response from the server
        console.log(data);
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

function loadResults() {
    // Construct the URL with parameters
    var url = `http://localhost:5001/load_results`;

    // Make a GET request to the server
    fetch(url)
    .then(response => response.json())
    .then(data => {
        // Process the response from the server
        console.log(data);
        loadWeightsFromServer(data['parameters'])
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

function mousePressed() {
    //if(mode == "draw") {
       notepad.trackMouse(true); 
    //}
    //if(mode == "display")  indDataset++;
  }
  
  function mouseReleased() {
    //if(mode == "draw") {
      notepad.trackMouse(false);
    //}
  }
  
  function keyPressed() {
    if (key === "c" || key === "C") {
      notepad.clear();
    }
    
    if (key === "d" || key === "D") {
       if(display == "tiles") 
            display = "stretch";
        else if(display == "stretch") 
            display = "tiles";
    }
  }

  function cleanNotepad() {
    notepad.clear();
  }