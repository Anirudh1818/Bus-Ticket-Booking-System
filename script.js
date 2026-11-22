// Bus Ticket Booking System - website version
// works same as bus_booking.c, bookings are saved in localStorage

let routes = [
    { id: 1, from: "Delhi", to: "Manali", time: "20:00", type: "Volvo AC", fare: 1450 },
    { id: 2, from: "Delhi", to: "Jaipur", time: "06:30", type: "AC Seater", fare: 650 },
    { id: 3, from: "Chandigarh", to: "Shimla", time: "08:00", type: "Non AC", fare: 280 },
    { id: 4, from: "Delhi", to: "Dehradun", time: "22:30", type: "AC Seater", fare: 720 },
    { id: 5, from: "Chandigarh", to: "Manali", time: "21:00", type: "Volvo AC", fare: 1100 },
    { id: 6, from: "Delhi", to: "Agra", time: "07:00", type: "Non AC", fare: 390 }
];

let ROWS = 10;
let COLS = 4;
let MAX_BOOK = 5;
let colName = ["A", "B", "C", "D"];

let bookings = [];
let seats = [];          // 0 = free, 1 = booked, 2 = selected
let selectedSeats = [];

let saved = localStorage.getItem("busBookings");
if (saved != null) {
    bookings = JSON.parse(saved);
}

function saveData() {
    localStorage.setItem("busBookings", JSON.stringify(bookings));
}

function showSection(id) {
    let sections = document.getElementsByClassName("section");
    for (let i = 0; i < sections.length; i++) {
        sections[i].style.display = "none";
    }
    document.getElementById(id).style.display = "block";

    let buttons = document.querySelectorAll("nav button");
    for (let i = 0; i < buttons.length; i++) {
        buttons[i].classList.remove("active");
    }
    document.getElementById("btn-" + id).classList.add("active");

    if (id == "records") {
        showRecords();
    }
}

function findRoute(id) {
    for (let i = 0; i < routes.length; i++) {
        if (routes[i].id == id) {
            return routes[i];
        }
    }
    return null;
}

function showRoutes() {
    let rows = "";
    let options = "<option value=''>-- Select Route --</option>";

    for (let i = 0; i < routes.length; i++) {
        let r = routes[i];
        rows += "<tr><td>" + r.id + "</td><td>" + r.from + "</td><td>" + r.to + "</td><td>" + r.time +
            "</td><td>" + r.type + "</td><td>Rs " + r.fare + "</td>" +
            "<td><button class='small-btn' onclick='goBook(" + r.id + ")'>Book</button></td></tr>";
        options += "<option value='" + r.id + "'>" + r.from + " to " + r.to + " (" + r.time + ", " + r.type + ")</option>";
    }
    document.getElementById("routeTable").innerHTML = rows;
    document.getElementById("routeSelect").innerHTML = options;
}

// "Book" button in routes table
function goBook(id) {
    showSection("book");
    document.getElementById("routeSelect").value = id;
    hideSeats();
}

function hideSeats() {
    document.getElementById("seatArea").style.display = "none";
    selectedSeats = [];
}

// input type date gives 2026-12-20, C program saves 20/12/2026
function formatDate(d) {
    let p = d.split("-");
    return p[2] + "/" + p[1] + "/" + p[0];
}


/* ---------------- seats ---------------- */

function loadSeats() {
    let rid = document.getElementById("routeSelect").value;
    let d = document.getElementById("travelDate").value;

    if (rid == "" || d == "") {
        alert("Please select route and date first");
        return;
    }
    let date = formatDate(d);

    seats = [];
    for (let i = 0; i < ROWS; i++) {
        seats[i] = [0, 0, 0, 0];
    }
    for (let i = 0; i < bookings.length; i++) {
        let b = bookings[i];
        if (b.routeId == rid && b.date == date) {
            let row = parseInt(b.seat) - 1;              // "10D" -> 10
            let col = colName.indexOf(b.seat.slice(-1));  // last letter
            seats[row][col] = 1;
        }
    }

    selectedSeats = [];
    drawSeats();
    drawPassengerForms();
    document.getElementById("seatArea").style.display = "block";
    document.getElementById("bookMsg").innerHTML = "";
}

function drawSeats() {
    let html = "<div class='driver'>FRONT &nbsp;&#128663; Driver</div>";
    let free = 0;

    for (let i = 0; i < ROWS; i++) {
        html += "<div class='seat-row'>";
        for (let j = 0; j < COLS; j++) {
            if (j == 2) {
                html += "<div class='aisle'></div>";
            }
            let label = (i + 1) + colName[j];

            if (seats[i][j] == 1) {
                html += "<div class='seat booked' title='Booked'>" + label + "</div>";
            } else {
                free++;
                let cls = "seat";
                if (seats[i][j] == 2) cls = "seat selected";
                html += "<div class='" + cls + "' onclick='selectSeat(" + i + "," + j + ")'>" + label + "</div>";
            }
        }
        html += "</div>";
    }

    document.getElementById("busBox").innerHTML = html;
    document.getElementById("seatInfo").innerHTML = "Seats available: <b>" + free + "</b> / " + (ROWS * COLS);
}

function selectSeat(i, j) {
    let label = (i + 1) + colName[j];

    if (seats[i][j] == 2) {
        // unselect
        seats[i][j] = 0;
        selectedSeats.splice(selectedSeats.indexOf(label), 1);
    } else {
        if (selectedSeats.length >= MAX_BOOK) {
            alert("You can book max " + MAX_BOOK + " seats at a time");
            return;
        }
        seats[i][j] = 2;
        selectedSeats.push(label);
    }
    drawSeats();
    drawPassengerForms();
}

function drawPassengerForms() {
    // remember what user already typed, otherwise it gets cleared on every click
    let old = {};
    for (let i = 0; i < selectedSeats.length; i++) {
        let s = selectedSeats[i];
        let n = document.getElementById("name_" + s);
        if (n != null) {
            old[s] = {
                name: n.value,
                age: document.getElementById("age_" + s).value,
                gender: document.getElementById("gender_" + s).value
            };
        }
    }

    if (selectedSeats.length == 0) {
        document.getElementById("passengerForms").innerHTML = "<p class='note'>Click on the seats you want (max " + MAX_BOOK + ")</p>";
        document.getElementById("totalFare").innerHTML = "";
        return;
    }

    let html = "";
    for (let i = 0; i < selectedSeats.length; i++) {
        let s = selectedSeats[i];
        let o = old[s] || { name: "", age: "", gender: "M" };
        html += "<div class='pass-box'><b>Seat " + s + "</b>" +
            "<input type='text' id='name_" + s + "' placeholder='Passenger name' maxlength='30' value='" + o.name + "'>" +
            "<div class='row'>" +
            "<input type='number' id='age_" + s + "' placeholder='Age' value='" + o.age + "'>" +
            "<select id='gender_" + s + "'><option value='M'>Male</option><option value='F'>Female</option><option value='O'>Other</option></select>" +
            "</div></div>";
    }
    document.getElementById("passengerForms").innerHTML = html;

    // put back the gender also
    for (let i = 0; i < selectedSeats.length; i++) {
        let s = selectedSeats[i];
        if (old[s]) {
            document.getElementById("gender_" + s).value = old[s].gender;
        }
    }

    let r = findRoute(document.getElementById("routeSelect").value);
    document.getElementById("totalFare").innerHTML = "Total: Rs " + (r.fare * selectedSeats.length) +
        " (" + selectedSeats.length + " x " + r.fare + ")";
}


/* ---------------- booking ---------------- */

function bookTicket() {
    let rid = parseInt(document.getElementById("routeSelect").value);
    let date = formatDate(document.getElementById("travelDate").value);
    let phone = document.getElementById("phone").value.trim();
    let r = findRoute(rid);

    if (selectedSeats.length == 0) {
        alert("Please select at least one seat");
        return;
    }

    let list = [];
    for (let i = 0; i < selectedSeats.length; i++) {
        let s = selectedSeats[i];
        let name = document.getElementById("name_" + s).value.trim();
        let age = parseInt(document.getElementById("age_" + s).value);
        let gender = document.getElementById("gender_" + s).value;

        if (name == "" || isNaN(age) || age < 1 || age > 110) {
            alert("Please fill correct details for seat " + s);
            return;
        }
        name = name.replace(/\|/g, " ");   // | is used as separator in the file

        list.push({ ticketNo: 0, routeId: rid, date: date, seat: s, name: name, age: age, gender: gender, phone: phone, fare: r.fare });
    }

    if (phone.length != 10 || isNaN(phone)) {
        alert("Please enter a valid 10 digit phone number");
        return;
    }

    let total = r.fare * list.length;
    if (!confirm("Total fare is Rs " + total + ". Confirm booking?")) {
        return;
    }

    // next ticket number = biggest one + 1
    let tno = 5000;
    for (let i = 0; i < bookings.length; i++) {
        if (bookings[i].ticketNo > tno) tno = bookings[i].ticketNo;
    }
    tno++;

    for (let i = 0; i < list.length; i++) {
        list[i].ticketNo = tno;
        bookings.push(list[i]);
    }
    saveData();

    hideSeats();
    document.getElementById("phone").value = "";
    document.getElementById("bookMsg").innerHTML = "<p class='success'>Booking confirmed!!</p>" + ticketHTML(list, false);
}

function ticketHTML(list, showCancel) {
    let r = findRoute(list[0].routeId);
    let rows = "";
    let total = 0;

    for (let i = 0; i < list.length; i++) {
        rows += "<tr><td>" + list[i].seat + "</td><td>" + list[i].name + "</td><td>" + list[i].age + "</td><td>" + list[i].gender + "</td></tr>";
        total += list[i].fare;
    }

    let html = "<div class='ticket'>" +
        "<div class='ticket-top'><span>&#128652; BUS TICKET</span><span>Ticket No. " + list[0].ticketNo + "</span></div>" +
        "<div class='ticket-body'>" +
        "<p><b>" + r.from + " &rarr; " + r.to + "</b></p>" +
        "<p>Date: " + list[0].date + " &nbsp;|&nbsp; Departure: " + r.time + " &nbsp;|&nbsp; " + r.type + "</p>" +
        "<p>Contact: " + list[0].phone + "</p>" +
        "<table><tr><th>Seat</th><th>Name</th><th>Age</th><th>Gender</th></tr>" + rows + "</table>" +
        "<p class='fare'>Total Fare: Rs " + total + "</p>" +
        "</div>" +
        "<div class='ticket-btns'><button class='small-btn' onclick='printTicket(this)'>Print</button>";
    if (showCancel) {
        html += "<button class='small-btn red' onclick='cancelTicket(" + list[0].ticketNo + ")'>Cancel Ticket</button>";
    }
    html += "</div></div>";
    return html;
}

function printTicket(btn) {
    let ticket = btn.parentElement.parentElement;
    ticket.classList.add("to-print");
    window.print();
    ticket.classList.remove("to-print");
}


/* ---------------- my booking ---------------- */

function searchBooking() {
    let q = document.getElementById("searchBox").value.trim();
    let out = document.getElementById("searchResult");

    if (q == "") {
        out.innerHTML = "<p class='error'>Enter ticket no or phone number</p>";
        return;
    }

    // group seats by ticket number
    let found = {};
    let order = [];
    for (let i = 0; i < bookings.length; i++) {
        let b = bookings[i];
        if (b.ticketNo == q || b.phone == q) {
            if (found[b.ticketNo] == undefined) {
                found[b.ticketNo] = [];
                order.push(b.ticketNo);
            }
            found[b.ticketNo].push(b);
        }
    }

    if (order.length == 0) {
        out.innerHTML = "<p class='error'>No booking found!</p>";
        return;
    }

    let html = "";
    for (let i = 0; i < order.length; i++) {
        html += ticketHTML(found[order[i]], true);
    }
    out.innerHTML = html;
}

function cancelTicket(tno) {
    let total = 0;
    let count = 0;
    for (let i = 0; i < bookings.length; i++) {
        if (bookings[i].ticketNo == tno) {
            total += bookings[i].fare;
            count++;
        }
    }

    if (!confirm("Cancel ticket " + tno + " (" + count + " seat/s)?")) {
        return;
    }

    let newList = [];
    for (let i = 0; i < bookings.length; i++) {
        if (bookings[i].ticketNo != tno) {
            newList.push(bookings[i]);
        }
    }
    bookings = newList;
    saveData();

    let refund = total - Math.floor(total * 10 / 100);
    document.getElementById("searchResult").innerHTML =
        "<p class='success'>Ticket " + tno + " cancelled. Refund amount: Rs " + refund + " (10% cancellation charge)</p>";
}


/* ---------------- all records ---------------- */

function showRecords() {
    let rows = "";
    let total = 0;

    if (bookings.length == 0) {
        document.getElementById("recordTable").innerHTML = "<tr><td colspan='9'>No bookings yet.</td></tr>";
        document.getElementById("recordTotal").innerHTML = "";
        return;
    }

    for (let i = 0; i < bookings.length; i++) {
        let b = bookings[i];
        let r = findRoute(b.routeId);
        rows += "<tr><td>" + b.ticketNo + "</td><td>" + r.from + "-" + r.to + "</td><td>" + b.date + "</td><td>" + b.seat +
            "</td><td>" + b.name + "</td><td>" + b.age + "</td><td>" + b.gender + "</td><td>" + b.phone + "</td><td>Rs " + b.fare + "</td></tr>";
        total += b.fare;
    }
    document.getElementById("recordTable").innerHTML = rows;
    document.getElementById("recordTotal").innerHTML = "<br><b>Total seats booked:</b> " + bookings.length +
        " &nbsp;|&nbsp; <b>Total collection:</b> Rs " + total;
}

// download records in same format as bus_bookings.txt of the C program
function downloadFile() {
    let text = "";
    for (let i = 0; i < bookings.length; i++) {
        let b = bookings[i];
        text += b.ticketNo + "|" + b.routeId + "|" + b.date + "|" + b.seat + "|" + b.name + "|" +
            b.age + "|" + b.gender + "|" + b.phone + "|" + b.fare + "\n";
    }
    let blob = new Blob([text], { type: "text/plain" });
    let a = document.createElement("a");
    a.href = URL.createObjectURL(blob);
    a.download = "bus_bookings.txt";
    a.click();
}

// load bus_bookings.txt made by the C program
function importFile() {
    let file = document.getElementById("fileInput").files[0];
    if (!file) {
        alert("Please choose the bus_bookings.txt file first");
        return;
    }

    let reader = new FileReader();
    reader.onload = function (e) {
        let lines = e.target.result.split("\n");
        bookings = [];

        for (let i = 0; i < lines.length; i++) {
            let line = lines[i].trim();
            if (line == "") continue;

            let p = line.split("|");
            if (p.length != 9) continue;        // skip broken lines
            if (findRoute(p[1]) == null) continue;

            bookings.push({
                ticketNo: parseInt(p[0]),
                routeId: parseInt(p[1]),
                date: p[2],
                seat: p[3],
                name: p[4],
                age: parseInt(p[5]),
                gender: p[6],
                phone: p[7],
                fare: parseInt(p[8])
            });
        }
        saveData();
        showRecords();
        alert(bookings.length + " records loaded from file");
    };
    reader.readAsText(file);
}


// when page loads
showRoutes();
showSection("routes");
document.getElementById("travelDate").min = new Date().toISOString().split("T")[0];
