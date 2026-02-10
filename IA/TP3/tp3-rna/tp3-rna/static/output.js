class OutputDisplay {
    constructor(x, y, w, h) {
      this.x = x;
      this.y = y;
      this.w = w;
      this.h = h;
    }
  
    display() {
      fill(0);
      stroke(255);
      strokeWeight(0);
      rect(this.x, this.y, this.w, this.h);
  
      textAlign(LEFT, TOP);
      textSize(16);
      noStroke();
      let str;
      let x = this.x + 8;
      let y = this.y + 8;
      let value;
      for (let i = 0; i < 10; i++) {
  
        if (i == indexOfHighestValue(currentGuess) && !notepad.empty) {
          fill(255);
        } else {
          fill(100);
        }
        value = round(currentGuess[i] * 1000) / 10
        str = i + " (" + value + "%)";
        text(str, x, y);
        y += 22;
      }
    }
  }

  class Marker {
  constructor() {
    this.w = 15;
    this.h = 15;
    this.y = nn.nodes[nn.nodes.length - 1][0].y;
    this.x = nn.nodes[nn.nodes.length - 1][0].x;

    this.vel = 0;
    this.force = 1;
    this.maxV = 8;
  }

  update() {
    this.yDesired = nn.nodes[nn.nodes.length - 1][indexOfHighestValue(currentGuess)].y;

    // dtermine distance
    let dist = this.yDesired - this.y;

    // add acceleration force to velocity
    this.vel += (this.force * sign(dist));
    
    // clamp velocity
    if (abs(this.vel) > this.maxV) this.vel = sign(this.vel) * this.maxV;

    // if distance smaller then calculated velocity set y to desired y
    if (abs(dist) <= abs(this.vel)) {
      this.y = this.yDesired;
      this.vel = 0;
    } else {
      this.y += this.vel;
    }
  }

  display() {
    noFill();
    stroke(255, 255, 0);
    strokeWeight(2);
    rect(this.x - this.w / 2, this.y - this.h / 2, this.w, this.h);
  }
}