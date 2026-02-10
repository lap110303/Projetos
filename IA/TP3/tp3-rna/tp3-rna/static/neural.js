class NeuralNetwork {
    constructor(structure) {
      this.structure = structure; // stores number of nodes for each layer
      this.nLayers = this.structure.length;
  
      this.weights = new Array(this.nLayers - 1); // stores weights of each layer
      for (let i = 0; i < this.weights.length; i++) {
        this.weights[i] = new Matrix(this.structure[i + 1], this.structure[i]);
        this.weights[i].randomize(0, 1, "float", 0.01);
      }
  
      this.biases = new Array(this.nLayers - 1);
      for (let i = 0; i < this.biases.length; i++) {
        this.biases[i] = new Matrix(this.structure[i + 1], 1);
        this.biases[i].randomize(0, 1, "float", 0.01);
      }
  
      this.learningRate = 0.1;
    }
  
  
    feedForward(inputArray, targetLayer) {
      let weights = this.weights[targetLayer - 1];
      let biases = this.biases[targetLayer - 1];
  
      let inputs = Matrix.fromArray(inputArray);
      let outputs = Matrix.dot(weights, inputs);
      outputs.add(biases);
  
      return outputs.toArray();
    }
  
    train(inputArray, targetArray) {
      let input = inputArray; // gets overwritten in each feedForward iteration
      let outputs_unmapped = [];
      let outputs = [];
  
      let errors = new Array(this.nLayers - 1); // create array with fixed length because it gets filled backwards
  
      // feedForward through all layers
      for (let i = 0; i < this.nLayers - 1; i++) {
        outputs_unmapped.push(this.feedForward(input, i + 1));
        outputs.push(Matrix.mapArray(outputs_unmapped[i], sigmoid));
        input = outputs[i];
      }
  
      // convert arrays to matrix objects
      for (let i = 0; i < outputs.length; i++) {
        outputs[i] = Matrix.fromArray(outputs[i]);
      }
      let targets = Matrix.fromArray(targetArray);
  
      // calculate final errors
      errors[errors.length - 1] = Matrix.subtract(targets, outputs[outputs.length - 1]);
  
      let weights_trans;
      let output_trans;
      let gradients;
      let deltas;
  
      for (let i = this.nLayers - 2; i >= 0; i--) {
        // calculate errors
        if (i < this.nLayers - 2) {
          weights_trans = Matrix.transpose(this.weights[i + 1]);
          errors[i] = Matrix.dot(weights_trans, errors[i + 1]);
        }
  
        // calculate gradients
        gradients = Matrix.fromArray(Matrix.mapArray(outputs_unmapped[i], dSigmoid));
        gradients.multiply(errors[i]);
        gradients.multiply(this.learningRate);
  
        // calculate deltas
        if (i > 0) {
          output_trans = Matrix.transpose(outputs[i - 1]);
        } else {
          output_trans = Matrix.transpose(Matrix.fromArray(inputArray));
        }
        deltas = Matrix.dot(gradients, output_trans);
  
        // adjust weights by deltas
        this.weights[i].add(deltas);
  
        // adjust biases by deltas
        this.biases[i].add(gradients);
      }
    }
  
    guess(inputArray) {
      let input = inputArray;
      let output_unmapped;
      let output;
  
      // feedForward through all layers
      for (let i = 0; i < this.nLayers - 1; i++) {
        output_unmapped = this.feedForward(input, i + 1);
        output = Matrix.mapArray(output_unmapped, relu);
        input = output;
        
        if (this.visualize) { 
          if(i == this.nLayers - 2) {
            this.layerOutputs[i] = this.normalizeArray(output);
          }
          else {
            this.layerOutputs[i] = output;
          }
        }
      }
      
      return this.layerOutputs[this.layerOutputs.length-1];
    }
    
    normalizeArray(array){
       let sum = 0;
      for(let i = 0; i < array.length; i++){
        sum+= array[i];
      }
    let result = [];
      for(let i = 0; i < array.length; i++){
       result.push(array[i]/sum) 
      }
      return result;
    }
  
    optimize(factor) {
      // copy to nn project
      for (let i = 0; i < this.weights.length; i++) {
        for (let j = 0; j < this.weights[i].data.length; j++) {
          for (let k = 0; k < this.weights[i].data[j].length; k++) {
            this.weights[i].data[j][k] = round(this.weights[i].data[j][k] * factor) / factor;
          }
        }
      }
      print("weights optimized (factor " + factor + ")")
    }
  
    exportWeightsToCSV() {
      // move to nn project
      let table;
      let newRow;
      for (let i = 0; i < this.weights.length; i++) {
        table = new p5.Table();
        table.addColumn('data');
        for (let j = 0; j < this.weights[i].data.length; j++) {
          newRow = table.addRow();
          newRow.setNum('data', table.getRowCount() - 1);
          newRow.setString('data', this.weights[i].data[j]);
        }
        saveTable(table, 'weights' + i + '.csv');
      }
      print("weights exported");
    }
  
    setWeights(weights) {
      // the argument "weights" is an array that contains for each interlayer an object with the weights
      let weights_string, weight;
  
      // go through interlayers
      for (let i = 0; i < this.weights.length; i++) {
  
        // go through nodes on the right side of interlayer i
        for (let j = 0; j < this.weights[i].data.length; j++) {
  
          // choose the weights object that corresponds to the current interlayer i and turn the string that represents the weights for the current node j into a string array
          weights_string = weights[i][j + 1].split(',');
  
          // go through connections of each node j
          for (let k = 0; k < this.weights[i].data[j].length; k++) {
  
            // convert string representation of corresponding weight k into a float and set weight
            weight = parseFloat(weights_string[k], 10);
            this.weights[i].data[j][k] = weight;
          }
        }
      }
      print("weights set");
    }

    setWeightsFromMatrix(weights) {
      // the argument "weights" is an array that contains for each interlayer an object with the weights
      let weight;
  
      // go through interlayers
      for (let i = 0; i < this.weights.length; i++) {
  
        // go through nodes on the right side of interlayer i
        for (let j = 0; j < this.weights[i].data.length; j++) {
  
          // go through connections of each node j
          for (let k = 0; k < this.weights[i].data[j].length; k++) {
  
            // convert string representation of corresponding weight k into a float and set weight
            weight = weights[i].data[j][k];
            this.weights[i].data[j][k] = weight;
          }
        }
      }
      print("weights set");
    }
  
    setBiases(biases) {
      // the argument "biases" is an array that contains for each interlayer an object with the biases
      let biases_string, bias;
  
      // go through interlayers
      for (let i = 0; i < this.biases.length; i++) {
  
        // go through nodes on the right side of interlayer i
        for (let j = 0; j < this.biases[i].data.length; j++) {
  
          // choose the biases object that corresponds to the current interlayer i and turn the string that represents the biases for the current node j into a string array
          biases_string = biases[i][j + 1].split(',');
  
          // go through connections of each node j
          for (let k = 0; k < this.biases[i].data[j].length; k++) {
            // convert string representation of corresponding bias k into a float and set bias
            bias = parseFloat(biases_string[k], 10);
            this.biases[i].data[j][k] = bias;
          }
        }
      }
      print("biases set");
    }

    setBiasesFromMatrix(biases) {
      // the argument "biases" is an array that contains for each interlayer an object with the biases
      let bias;
  
      // go through interlayers
      for (let i = 0; i < this.biases.length; i++) {
  
        // go through nodes on the right side of interlayer i
        for (let j = 0; j < this.biases[i].data.length; j++) {
  
          // go through connections of each node j
          for (let k = 0; k < this.biases[i].data[j].length; k++) {
            // convert string representation of corresponding bias k into a float and set bias
            bias = biases[i].data[j][k];
            this.biases[i].data[j][k] = bias;
          }
        }
      }
      print("biases set");
    }
  
    initVisualization(x, y, w, h, displayInputLayer) {
      this.visualize = true;
      this.x = x;
      this.y = y;
      this.h = h;
      this.w = w;
      this.displayInputLayer = displayInputLayer;
  
      if (this.displayInputLayer) {
        this.nShownLayers = this.nLayers;
        this.minLayer = 0;
      } else {
        this.nShownLayers = this.nLayers - 1;
        this.minLayer = 1;
      }
      let maxNNodes = findMax(this.structure.slice(this.minLayer), this.structure.length);
      let dXLayer = w / (this.nShownLayers - 1);
      let dYNode = h / (maxNNodes - 1);
      let offsetY;
  
      // data for visualization
      if (displayInputLayer) {
        this.inputs = [];
        for (let i = 0; i < this.structure[0]; i++) {
          this.inputs[i] = 0;
        }
      }
  
      this.layerOutputs = [];
      for (let i = 0; i < this.nLayers - 1; i++) {
        this.layerOutputs[i] = [];
        for (let j = 0; j < this.structure[i + 1]; j++) {
          this.layerOutputs[i][j] = 0;
        }
      }
  
      // populate node array
      this.nodes = [];
      for (let i = 0; i < this.nShownLayers; i++) { // goes through layers
        this.nodes[i] = [];
        offsetY = (h - (this.structure[i + this.minLayer] - 1) * dYNode) / 2;
  
        for (let j = 0; j < this.structure[i + this.minLayer]; j++) {
          this.nodes[i].push(new Node(x + i * dXLayer, y + offsetY + j * dYNode));
        }
      }
  
      // populate connections array
      this.connections = [];
      for (let i = 0; i < this.nShownLayers - 1; i++) { // loop through "between"-layers
        this.connections[i] = new Matrix(this.structure[i + this.minLayer + 1], this.structure[i + this.minLayer]);
  
        for (let j = 0; j < this.structure[i + this.minLayer + 1]; j++) { // loop through sink nodes
          for (let k = 0; k < this.structure[i + this.minLayer]; k++) { // loop through source nodes
            this.connections[i].data[j][k] = new Connection(this.nodes[i][k], this.nodes[i + 1][j]);
  
          }
        }
      }
    }
  
    display(maxOpac) {
      fill(0);
      stroke(255);
      strokeWeight(0);
      rect(this.x-15, this.y-15, this.w+30, this.h+30);
      
      // display connections
      let weight, clr, strength;
      for (let i = 0; i < this.connections.length; i++) { // loop through "between"-layers
        for (let j = 0; j < this.structure[i + this.minLayer + 1]; j++) { // loop through sink nodes
          for (let k = 0; k < this.structure[i + this.minLayer]; k++) {
            weight = this.weights[i + this.minLayer].data[j][k];
            strength = map(abs(weight), 0, 1, 0, maxOpac);
            
            if (weight < 0) {
              clr = color(0, 0, 255, strength);
            } else {
              clr = color(255, 0, 0, strength);
            }

            this.connections[i].data[j][k].display(clr);
          }
        }
      }
  
      // display nodes
      if (this.displayInputLayer) {
        // input nodes
        for (let i = 0; i < this.structure[0]; i++) {
  
          strength = map(abs(this.inputs[i]), 0, 1, 0, 255);
          this.nodes[0][i].display(strength);
        }
      }
  
      // layer nodes
      for (let i = 1; i < this.nLayers; i++) {
        for (let j = 0; j < this.structure[i]; j++) {
          strength = map(this.layerOutputs[i - 1][j], 0, 1, 0, 255);
          this.nodes[i-this.minLayer][j].display(strength);
        }
      }
      //this.displayLabels();
    }
  
    displayLabels() {
      textSize(12);
      textAlign(RIGHT, CENTER);
      fill(255);
      noStroke();
  
      for (let i = 0; i < this.nodes[0].length; i++) {
        text(this.inputLabels[i], this.nodes[0][i].x - 9, this.nodes[0][i].y);
      }
  
      textAlign(LEFT, CENTER);
      for (let i = 0; i < this.nodes[this.nodes.length - 1].length; i++) {
        text(this.outputLabels[i], this.nodes[this.nodes.length - 1][i].x + 9, this.nodes[this.nodes.length - 1][i].y);
      }
    }
  }
  
  class Node {
    constructor(x, y) {
      this.x = x;
      this.y = y;
      this.d = 10;
    }
  
    display(strength) {
      if (strength != undefined) {
        fill(strength)
      } else {
        fill(0);
      }
      stroke(100);
      strokeWeight(1);
      circle(this.x, this.y, this.d);
    }
  }
  
  class Connection {
    constructor(nodeA, nodeB) {
      this.x1 = nodeA.x;
      this.x2 = nodeB.x;
      this.y1 = nodeA.y;
      this.y2 = nodeB.y;
    }
  
    display(clr) {
      stroke(clr);
      strokeWeight(1);
      line(this.x1, this.y1, this.x2, this.y2);
    }
  }