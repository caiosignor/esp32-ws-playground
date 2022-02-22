// https://www.codebrainer.com/blog/tic-tac-toe-javascript-game

const X_CLASS = 'x'
const CIRCLE_CLASS = 'circle'

const cellElements = document.querySelectorAll('[data-cell]')
const board = document.getElementById('board')
const winningMessageElement = document.getElementById('winningMessage')
const restartButton = document.getElementById('restartButton')
const winningMessageTextElement = document.querySelector('[data-winning-message-text]')

startGame()
initWebSocket();

restartButton.addEventListener('click', restartGame)

var mySymbol;

function restartGame() {
  var msg = {
    type: 'restart',
  }
  websocket.send(JSON.stringify(msg));
}

function startGame() {
  circleTurn = false
  cellElements.forEach(cell => {
    cell.classList.remove(X_CLASS)
    cell.classList.remove(CIRCLE_CLASS)
    cell.removeEventListener('click', handleClick)
    cell.addEventListener('click', handleClick, { once: true })
  })
  setBoardHoverClass()
  winningMessageElement.classList.remove('show')
}

function handleClick(e) {
  const cell = e.target
  var id = cell.id;

  var i, j;
  if (id < 3) {
    i = 0;
    j = parseInt(id);
  } else if (id < 6) {
    i = 1;
    j = id - 3;
  } else {
    i = 2;
    j = id - 6;
  }
  console.log(i, j);
  placeMark(i, j)
}

function endGame(draw) {
  winningMessageTextElement.innerText = `${draw == CIRCLE_CLASS ? "O's" : "X's"} Wins!`
  winningMessageElement.classList.add('show')
}

function isDraw() {
  return [...cellElements].every(cell => {
    return cell.classList.contains(X_CLASS) || cell.classList.contains(CIRCLE_CLASS)
  })
}

function placeMark(i, j) {
  var msg = {
    type: 'place',
    payload: {
      i: i,
      j: j
    }
  }
  console.log(msg);
  websocket.send(JSON.stringify(msg));
}

function setBoardHoverClass() {

  board.classList.add(mySymbol);
}

var websocket;

function initWebSocket() {
  var gateway = `ws://${window.location.hostname}/ws`;
  console.log('Trying to open a WebSocket connection... ' + gateway);
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
  console.log('Connection opened');
}

function onClose(event) {
  console.log('Connection closed');
}

function getSymbol(symbol) {
  switch (symbol) {
    case 120: return X_CLASS; break;
    case 111: return CIRCLE_CLASS; break;
  }
}

function onMessage(event) {
  var json = JSON.parse(event.data);
  console.log(json);
  if (json.type == 'gameboard') {
    var table = json.payload.table;
    for (index in table) {
      cellElements[index].classList.add(getSymbol(table[index]));
    }
  }
  else if (json.type == 'winner') {
    var symbol = getSymbol(json.payload.symbol);
    endGame(symbol);
  }
  else if (json.type == 'restart') {
    startGame();
  }
  else if(json.type == 'symbol')
  {
    mySymbol = getSymbol(json.payload.symbol);
    setBoardHoverClass();
  }

}