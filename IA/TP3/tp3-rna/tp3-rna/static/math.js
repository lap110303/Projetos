function sigmoid(x) {
  // calculates sigmoid function
    return 1 / (1 + Math.exp(-x));
}

function relu(x) {
  // calculates relu function
  return Math.max(0, x);
}

function dSigmoid(x) {
  // calculates derivative of sigmoid function
  return sigmoid(x) * (1 - sigmoid(x));
}

function sign(value){
  if (value >= 0) 
    return +1;
  else 
    return -1;
}

function indexOfHighestValue(array) {
  let highestValue = -Infinity;
  let result = 0;
  for (let i = 0; i < array.length; i++) {
    if (array[i] > highestValue) {
      highestValue = array[i];
      result = i;
    }
  }
  return result;
}
  
function normalizeArray(array, max) {
  let result = [];
  for (let i = 0; i < array.length; i++) {
    result.push(array[i] / max);
  }
  return result;
}
  
function findMax(array) {
  let max = 0;
  for (let i = 0; i < array.length; i++) {
    if (array[i] > max) {
      max = array[i];
    }
  }
  return max;
}
