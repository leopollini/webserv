const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const port = 3000;

app.use(bodyParser.urlencoded({ extended: false }));

app.set('view engine', 'ejs');
app.use(express.static('public'));

// Dummy data for testing
let items = [
    { id: 1, name: 'Item 1' },
    { id: 2, name: 'Item 2' },
];

// Routes
app.get('/items', (req, res) => {
    res.render('index', { items });
});


app.post('/items', (req, res) => {
    // Create a new item (in-memory)
    const newItem = {
        id: items.length + 1,
        name: req.body.name,
    };

    items.push(newItem);
    res.redirect('/items');
});

app.delete('/deleteitems',(req,res)=>{
    const id = parseInt(req.body.id);

    items = items.filter(item => item.id !== id);

    res.redirect('/items');
})

app.listen(port, () => {
    console.log(`Server is running on port ${port}`);
});