class Notepad {
    constructor(x, y, cellSize, nCellsX, nCellsY) {
      this.x = x;
      this.y = y;
  
      
      this.cellSize = cellSize;
  
      this.nCellsX = nCellsX;
      this.nCellsY = nCellsY;
  
      this.w = cellSize * this.nCellsX;
      this.h = cellSize * this.nCellsY;
  
      this.img = createImage(this.nCellsX, this.nCellsY);
  
      this.clear();
    }
  
    clear() {
      this.cells = [];
      this.empty = true;
      this.overFlownCells = [];
      
      for (let i = 0; i < this.nCellsX * this.nCellsY; i++) {
        this.cells.push(0);
        this.overFlownCells.push(false);
      }
    }
  
    trackMouse(bool) {
      this.mouseTracked = bool;
      this.lastIndex = undefined;
    }
  
    update(x, y) {
      this.hover = false;
      if(this.inside(x, y)){
        let index = this.findCellIndex(x, y);
         this.hover = true;
        this.indexHover = index;
         
      if (this.mouseTracked) {
        
        if (this.lastIndex != undefined) {
          this.drawLine(index);
        } else if (this.lastIndex == undefined) {
          this.cells[index] += 255;
        }
        this.empty = false;
        //this.overflow(70, 30);
  
        this.lastIndex = index;
      }}
    }
  
    drawLine(currentIndex) {
      let x0 = this.getIndexXFromIndex(currentIndex);
      let y0 = this.getIndexYFromIndex(currentIndex);
      let x1 = this.getIndexXFromIndex(this.lastIndex);
      let y1 = this.getIndexYFromIndex(this.lastIndex);
  
      let line = XiaolinWu.plot(x0, y0, x1, y1);
      let indX, indY, ind;
      let dy, dx, dist, clr, maxClr, overflowA, overflowB;
  
      for (let i = 0; i < line.length; i++) {
        indX = line[i].x;
        indY = line[i].y;
  
        dx = x1 - x0;
        dy = y1 - y0;
        
        dist = sqrt(dx * dx + dy * dy)
  
        maxClr = map(dist, 2, 10, 255, 50);
        clr = map(line[i].b, 0, 1, 0, maxClr);
  
        ind = this.getIndex(indX, indY);
        this.cells[ind] += clr;
        
        overflowA = clr/2;//clr* 0.7 * 1/dist  // clr/2
        overflowB = clr/5;//clr * 0.3 * 1/dist  // clr/5
        
        if (this.overFlownCells[ind] == false) this.overflow(ind, overflowA, overflowB);
      }
    }
  
    overflow(cellIndex, flowA, flowB) {
      let indX, indY, cell;
      for (let dX = -1; dX <= 1; dX++) {
        for (let dY = -1; dY <= 1; dY++) {
          indX = this.getIndexXFromIndex(cellIndex);
          indY = this.getIndexYFromIndex(cellIndex);
  
          cell = this.getIndex(indX + dX, indY + dY);
          if (dX == 0 || dY == 0) {
            this.cells[cell] += flowA;
          } else {
            this.cells[cell] += flowB;
          }
          this.overFlownCells[cellIndex] = true;
        }
      }
    }
  
    /*
    overflow(flowA, flowB){
      let cell, indX, indY;
        for (let i = 0; i < this.cells.length; i++) {
          if (this.cells[i] >= 255 && this.overFlownCells[i] == false) {
            for (let dX = -1; dX <= 1; dX++) {
              for (let dY = -1; dY <= 1; dY++) {
                indX = this.getIndexXFromIndex(i);
                indY = this.getIndexYFromIndex(i);
                
                cell = this.getIndex(indX + dX, indY + dY);
                if (dX == 0 || dY == 0) {
                  this.cells[cell]+=flowA;
                }
                else {
                  this.cells[cell] += flowB;
                }
                this.overFlownCells[i] = true;
              }
            }
          }
          
        }
    }*/
  
    getIndex(indexX, indexY) {
      if (indexX < 0 || indexX > this.nCellsX - 1 || indexY < 0 || indexY > this.nCellsY - 1) {
        return undefined;
      } else {
        return indexY * this.nCellsX + indexX;
      }
    }
  
    getIndexXFromIndex(index) {
      return index % this.nCellsX;
    }
  
    getIndexYFromIndex(index) {
      return floor(index / this.nCellsX);
    }
  
    inside(x, y) {
      if (x >= this.x && x <= this.x + this.w && y >= this.y && y <= this.y + this.h) return true;
      else return false;
    }
  
    findCellIndex(x, y) {
      let xNotepad = x - this.x;
      let yNotepad = y - this.y;
  
      let indexX = floor(xNotepad / this.cellSize);
      let indexY = floor(yNotepad / this.cellSize);
      let index = this.nCellsY * indexY + indexX;
      return index;
    }
  
    display() {
    
    if(display == "tiles"){
      noStroke();
      let localX, localY;
      for (let i = 0; i < this.cells.length; i++) {
        localX = (i % this.nCellsX) * this.cellSize;
        localY = floor(i / this.nCellsX) * this.cellSize;
        
        fill(this.cells[i]);
        
        rect(this.x + localX, this.y + localY, this.cellSize, this.cellSize);
      }
      if(this.hover){
        let x = (this.indexHover % this.nCellsX) * this.cellSize;
        let y = floor(this.indexHover / this.nCellsX) * this.cellSize;
        noFill();
        stroke(255, 0, 0, 100);
        strokeWeight(1);
        rect(this.x+x, this.y+y, this.cellSize, this.cellSize);
      }
    }
      else {
      this.img.loadPixels();
      let pixelIndex = 0;
      for (let i = 1; i < this.cells.length; i++) {
        for (let j = 0; j < 4; j++) {
          if (j == 3) this.img.pixels[pixelIndex] = 255;
          else this.img.pixels[pixelIndex] = this.cells[i];
          pixelIndex++;
        }
      }
      this.img.pixels[this.img.pixels.length - 1] = 255;
      this.img.updatePixels();
      image(this.img, this.x, this.y, this.w, this.h)
      }
      
      stroke(255, 100);
    noFill();
    strokeWeight(1);
    // circle(this.x + this.w / 2, this.y + this.h / 2, this.h * 0.8);
      
      if(!this.empty){
      line(this.x + this.w/2, this.y + this.h/2-15, this.x + this.w/2, this.y + this.h/2+15);
      line(this.x + this.w/2 - 15, this.y + this.h/2, this.x + this.w/2 + 15, this.y + this.h/2);
      }
      
      stroke(100);
      noFill();
      strokeWeight(1);
      rect(this.x, this.y, this.w, this.h);
    }
  
  }

  class XiaolinWu {
    // https://gist.github.com/polyamide/3f33cb4dc69e22fbf8b66cee39b78d60
    static integerPart(v) {
        let isNeg = (v < 0) ? -1 : 1;
        let abs = Math.abs(v);
        let integerPart = Math.floor(abs);

        return integerPart * isNeg;
    }

    static fraction(v) {
        if (v < 0) {
            return 1 - (v - Math.floor(v));
        }

        return v - Math.floor(v);
    }

    static reverseFraction(v) {
        return 1 - XiaolinWu.fraction(v);
    }

    static round(v) {
        return XiaolinWu.integerPart(v + 0.5);
    }

    static plot(x0, y0, x1, y1) {
        if (x0 == x1 && y0 == y1) {
            return []
        }

        let fpart = XiaolinWu.fraction;
        let rfpart = XiaolinWu.reverseFraction;
        let ipart = XiaolinWu.integerPart;
        let round = XiaolinWu.round;

        let dots = [];
        let steep = Math.abs(y1 - y0) > Math.abs(x1 - x0);

        if (steep) {
            [y0, x0] = [x0, y0];
            [y1, x1] = [x1, y1];
        }

        if (x0 > x1) {
            [x1, x0] = [x0, x1];
            [y1, y0] = [y0, y1];
        }

        let dx = x1 - x0;
        let dy = y1 - y0;
        let gradient = dy / dx;

        let xEnd = round(x0);
        let yEnd = y0 + gradient * (xEnd - x0);
        let xGap = rfpart(x0 + 0.5);
        let xPx1 = xEnd;
        let yPx1 = ipart(yEnd);

        if (steep) {
            dots.push({ x: yPx1, y: xPx1, b: rfpart(yEnd) * xGap });
            dots.push({ x: yPx1 + 1, y: xPx1, b: fpart(yEnd) * xGap });
        } else {
            dots.push({ x: xPx1, y: yPx1, b: rfpart(yEnd) * xGap });
            dots.push({ x: xPx1, y: yPx1 + 1, b: fpart(yEnd) * xGap });
        }

        let intery = yEnd + gradient;

        xEnd = round(x1);
        yEnd = y1 + gradient * (xEnd - x1);
        xGap = fpart(x1 + 0.5);

        let xPx2 = xEnd;
        let yPx2 = ipart(yEnd);

        if (steep) {
            dots.push({ x: yPx2, y: xPx2, b: rfpart(yEnd) * xGap });
            dots.push({ x: yPx2 + 1, y: xPx2, b: fpart(yEnd) * xGap });
        } else {
            dots.push({ x: xPx2, y: yPx2, b: rfpart(yEnd) * xGap });
            dots.push({ x: xPx2, y: yPx2 + 1, b: fpart(yEnd) * xGap });
        }

        if (steep) {
            for (let x = xPx1 + 1; x <= xPx2 - 1; x++) {
                dots.push({ x: ipart(intery), y: x, b: rfpart(intery) });
                dots.push({ x: ipart(intery) + 1, y: x, b: fpart(intery) });
                intery = intery + gradient;
            }
        } else {
            for (let x = xPx1 + 1; x <= xPx2 - 1; x++) {
                dots.push({ x: x, y: ipart(intery), b: rfpart(intery) });
                dots.push({ x: x, y: ipart(intery) + 1, b: fpart(intery) });
                intery = intery + gradient
            }
        }

        return dots;
    }
}