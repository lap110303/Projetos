// sketch.js (CLIENTE) - usando GET em vez de POST

const ROWS = 6, COLS = 7;
const EMPTY = 0;

let cellSize = 80, margin = 16;
let board, turn = 0, winner = 0, aiThinking = false;
let players = { 1: "", 2: "" };

const API = "http://localhost:5001/ai_move";

// ------------------------- p5.js SETUP -------------------------
function setup() 
{
  const w = COLS * cellSize + margin * 2;
  const h = ROWS * cellSize + margin * 2;
  let cnv = createCanvas(w, h);
  cnv.parent("mapCanvas");

  document.getElementById("newGame").onclick = newGame;
  newGame();
}

function draw() 
{
  background(255);
  drawGrid();
  drawChips();
  
  updateHUD();
}

function LoadPlayers() 
{
  const p1Select = document.getElementById("player1");
  const p2Select = document.getElementById("player2");
  
  players[1] = p1Select.value;
  players[2] = p2Select.value;

  // se o jogador atual for uma IA (qualquer string que comece com "ai")
  if (winner === 0 && players[turn] && players[turn].startsWith("ai")) {
    makeAIMove();
  }
}

// ------------------------- UI & ESTADO -------------------------
function newGame() 
{
  board = Array.from({ length: ROWS }, () => Array(COLS).fill(EMPTY));
  
  LoadPlayers();
  
  turn = 1;
  winner = 0;
  
  aiThinking = false;
  // Se o jogador inicial for IA (ex: "ai_minimax")
  if (players[turn] && players[turn].startsWith("ai")) {
    makeAIMove();
  }

  updateHUD();
  console.log("Novo jogo com jogadores:", players);
}

function updateHUD() 
{
  // Desabilitar interface se IA pensando
  document.getElementById("player1").disabled = aiThinking;
  document.getElementById("player2").disabled = aiThinking;
  document.getElementById("maxTimeMs").disabled = aiThinking;
  document.getElementById("newGame").disabled = aiThinking;
  document.getElementById("maxDepth").disabled = aiThinking;

  // Converter seleção para texto amigável
  function agentLabel(sel) {
    if (!sel || sel === "human") return sel === "human" ? "Humano" : "—";
    if (sel === "ai_random") return "IA (Aleatório)";
    if (sel === "ai_minimax") return "IA (Minimax)";
    if (sel === "ai_alphabeta") return "IA (Alfa-Beta)";
    if (sel === "ai_id") return "IA (Iterative Deepening)";
    return sel;
  }

  // Atualizar labels
  document.getElementById("turnLabel").innerText =
    winner !== 0 ? "—" :
    (turn === 1 ? (players[1] === "human" ? "P1 (vermelho)" : `P1 - ${agentLabel(players[1])}`) :
                  (players[2] === "human" ? "P2 (amarelo)" : `P2 - ${agentLabel(players[2])}`));

  // Atualizar vencedor
  document.getElementById("winnerLabel").innerText = 
    winner === 0 ? "—" :
    winner === -1 ? "Empate!" :
    (winner === 1 ? "P1 (Vermelho)" : "P2 (Amarelo)") + " venceu!";
}

// ------------------------ DESENHO DO TABULEIRO ------------------------
function drawGrid() 
{
  push();
  translate(margin, margin);

  // Desenhar fundo azul
  noStroke();
  fill(30, 70, 200);
  rect(0, 0, COLS * cellSize, ROWS * cellSize, 14);

  // Desenhar círculos vazios
  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      fill(240);
      circle(c * cellSize + cellSize / 2, r * cellSize + cellSize / 2, cellSize * 0.7);
    }
  }

  // Desenhar linhas de grade
  stroke(0, 50);
  strokeWeight(2);
  for (let r = 1; r < ROWS; r++) {
    line(0, r * cellSize, COLS * cellSize, r * cellSize);
  }
  for (let c = 1; c < COLS; c++) {
    line(c * cellSize, 0, c * cellSize, ROWS * cellSize);
  }

  // Highlight coluna sob o mouse se o turno for do jogador
  if (winner === 0 && players[turn] === "human" &&
      mouseX >= margin && mouseX <= width - margin &&
      mouseY >= margin && mouseY <= height - margin) {
    const col = Math.floor((mouseX - margin) / cellSize);
    if (col >= 0 && col < COLS) {
      noStroke();
      fill(255, 255, 255, 30);
      rect(col * cellSize, 0, cellSize, ROWS * cellSize);
    }
  }

  pop();
}

function drawChips() 
{
  push();
  translate(margin, margin);

  for (let r = 0; r < ROWS; r++) {
    for (let c = 0; c < COLS; c++) {
      const cell = board[r][c];
      if (cell === EMPTY) continue;

      if (cell === 1) {
        fill(220, 50, 30);  // vermelho
      } else if (cell === 2) {
        fill(240, 220, 70); // amarelo
      }

      circle(c * cellSize + cellSize / 2, r * cellSize + cellSize / 2, cellSize * 0.7);
    }
  }
  
  pop();
}

function changeTurn() 
{
  turn = turn === 1 ? 2 : 1;

  // Se o novo jogador for IA (começa com "ai"), dispara a jogada automática
  if (winner === 0 && players[turn] && players[turn].startsWith("ai")) {
    makeAIMove();
  }
}

// ------------------------ ENTRADA DO JOGADOR ------------------------
function mousePressed() 
{
  if (winner !== 0) return;

  if (players[turn] === "human" && !aiThinking) 
  {
    // Limitar jogada ao canvas
    if (mouseX < margin || mouseX > width - margin ||
        mouseY < margin || mouseY > height - margin) {
      return;
    }

    // Calcular coluna clicada
    const col = Math.floor((mouseX - margin) / cellSize);
    if (col >= 0 && col < COLS) 
    {
      if (applyMove(col, turn)) 
      {
        checkWinner();
        changeTurn();
      }
    }
  }
}

function applyMove(col, player)
{
  if (col < 0 || col >= COLS || board[0][col] !== EMPTY) return false;
  
  for (let r = ROWS - 1; r >= 0; r--) 
  {
    if (board[r][col] === EMPTY) 
    {
      board[r][col] = player;
      return true;
    }
  }
  
  return false;
}

function checkWinner() 
{
  const w = winnerOf(board);
  
  if (w !== 0) 
  {
    winner = w;
  }
  else if (board[0].every(x => x !== EMPTY)) 
  {
    winner = -1;
  }
}

// ------------------------ LÓGICA DO SERVIDOR GET ------------------------
function encodeBoard(board) 
{
  return board.map(row => row.join('')).join(';');
}

async function makeAIMove() 
{
  if (winner !== 0 || aiThinking) return;
  
  aiThinking = true;

  const boardStr = encodeBoard(board);
  const player = turn;

  const maxTimeMs = parseInt(document.getElementById("maxTimeMs").value);
  const maxDepth = parseInt(document.getElementById("maxDepth").value);

  // pega o agente selecionado para este jogador
  const agent = players[turn] || "ai_random";

  console.log(`Calling AI API with board=${boardStr}, player=${player}, agent=${agent}, max_time_ms=${maxTimeMs}, max_depth=${maxDepth}`);

  const url = `${API}?board=${encodeURIComponent(boardStr)}&player=${encodeURIComponent(player)}&max_time_ms=${encodeURIComponent(maxTimeMs)}&max_depth=${encodeURIComponent(maxDepth)}&agent=${encodeURIComponent(agent)}`;

  try {
    const response = await fetch(url);
    const data = await response.json();
    
    const col = data.col;
    if (applyMove(col, turn)) {
      aiThinking = false;
      checkWinner();
      changeTurn();
    } else {
      aiThinking = false;
      console.error("Jogada inválida retornada pela IA:", col);
    }
  } 
  catch (error) {
    aiThinking = false;
    console.error("Erro ao chamar a API da IA:", error);
  } 
  finally  {
    aiThinking = false;
  }
}

// ------------------------ FUNÇÕES AUXILIARES ------------------------
function winnerOf(bd) 
{
  // horizontais
  for (let r = 0; r < ROWS; r++)
    for (let c = 0; c < COLS - 3; c++) {
      const x = bd[r][c];
      if (x && x === bd[r][c + 1] && x === bd[r][c + 2] && x === bd[r][c + 3]) return x;
    }

  // verticais
  for (let c = 0; c < COLS; c++)
    for (let r = 0; r < ROWS - 3; r++) {
      const x = bd[r][c];
      if (x && x === bd[r + 1][c] && x === bd[r + 2][c] && x === bd[r + 3][c]) return x;
    }

  // diag ↘
  for (let r = 0; r < ROWS - 3; r++)
    for (let c = 0; c < COLS - 3; c++) {
      const x = bd[r][c];
      if (x && x === bd[r + 1][c + 1] && x === bd[r + 2][c + 2] && x === bd[r + 3][c + 3]) return x;
    }

  // diag ↗
  for (let r = 3; r < ROWS; r++)
    for (let c = 0; c < COLS - 3; c++) {
      const x = bd[r][c];
      if (x && x === bd[r - 1][c + 1] && x === bd[r - 2][c + 2] && x === bd[r - 3][c + 3]) return x;
    }

  return 0;
}