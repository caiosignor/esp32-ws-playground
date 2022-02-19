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

restartButton.addEventListener('click', startGame)

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
  if (draw) {
    winningMessageTextElement.innerText = 'Draw!'
  } else {
    winningMessageTextElement.innerText = `${circleTurn ? "O's" : "X's"} Wins!`
  }
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
  board.classList.remove(X_CLASS)
  board.classList.remove(CIRCLE_CLASS)
  if (circleTurn) {
    board.classList.add(CIRCLE_CLASS)
  } else {
    board.classList.add(X_CLASS)
  }
}

var websocket;

function initWebSocket() {
  var gateway = `ws://${window.location.hostname}/ws`;
  console.log('Trying to open a WebSocket connection... '+gateway);
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

function onMessage(event) {
  var json = JSON.parse(event.data);
  console.log(json);
  if (json.type == 'gameboard') {
    var table = json.payload.table;
    for(index in table)
    { 
      console.log(table[index]);
      if(table[index] == 120)
      {
        cellElements[index].classList.add(X_CLASS);
      }else if(table[index] == 111)
      {
        cellElements[index].classList.add(CIRCLE_CLASS);
      }
    }    
  }  
}