// Just save the contents of the main HTML page as a string in flash memory.
// Remember this can't be insanely big, there's only 512KB total flash!

// Note the raw string literal R syntax is C++11 only!
const char* index_html = R"(
  <!DOCTYPE html>
  <html>
    <head>
      <meta name ="viewport">
      <title>Morse Code Blinker</title>
      <style>
        html{font-family:sans-serif;-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}
        body {
          padding-top: 50px;
          padding-bottom: 20px;
          margin:0;
        }
        .container{margin-right:auto;margin-left:auto;padding-left:15px;padding-right:15px}
        @media (min-width:768px){.container{width:750px}}
        @media (min-width:992px){.container{width:970px}}
        @media (min-width:1200px){.container{width:1170px}}
        .container{color:inherit;background-color:#fff}
        .jumbotron{padding:30px;margin-bottom:30px;color:inherit;background-color:#428bca}
        .jumbotron h1,.jumbotron .h1{color:inherit}
        .jumbotron p{margin-bottom:15px;font-size:21px;font-weight:200}
        .container .jumbotron{border-radius:6px}
        .form-group{margin-bottom:15px}
        .form-group .form-control, .btn {padding:10px 16px;font-size:18px;border-radius:6px}
      </style>
    </head>
    <body>
      <div class="container">
        <div class="jumbotron">
          <h1>Morse Code Blinker</h1>
          <p>Submit text to have it blinked out as morse code on the LED!</p>
          <form action="/morse" method="post" enctype="application/x-www-form-urlencoded" target="/">
            <div class="form-group">
              <input class="form-control input-lg" type="text" id="message" name="message" placeholder="Enter message..." required="required" autocapitalize="characters" minlength=1 maxlength=500>
            </div>
            <button class="btn btn-lg btn-primary" type="submit">Blink it!</button>
          </form>
        </div>
      </div>
    </body>
  </html>
)";
