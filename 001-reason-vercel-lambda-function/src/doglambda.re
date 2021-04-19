open Vercel;

let (let.await) = Lwt.Syntax.( let* );

module Client = Piaf.Client.Oneshot;

let trimForwardSlash = str => String.sub(str, 1, String.length(str) - 1);

let getDogURL = breed =>
  Uri.of_string(
    Format.asprintf("https://dog.ceo/api/breed/%s/images/random", breed),
  );

let handleRequest: (Request.t, 'a) => Lwt_result.t(Response.t, string) =
  (request, _context) => {
    switch (request.target) {
    | ""
    | "/" =>
      /* "" or "/" */
      Lwt.return_ok(
        Response.of_string(
          ~body="Usage: <this-url>/<dog_breed>, e.g. <this-url>/labrador",
          `Not_found,
        ),
      )
    | path =>
      /* Get dog breed from request path, e.g. example.com/labrador */
      let breed = trimForwardSlash(path);
      let url = getDogURL(breed);
      let.await response =
        Client.get(
          ~config={
            ...Piaf.Config.default,
            allow_insecure:
              /* Kids: Don't try this at home. */
              true,
          },
          url,
        );

      switch (response) {
      | Ok({Piaf.Response.body, _}) =>
        let.await body_string = Piaf.Body.to_string(body);
        /*
          {
            message: 'https://images.dog.ceo/breeds/labrador/IMG_4709.jpg',
            status: 'success'
          }
         */
        let body_json = Yojson.Basic.from_string(Result.get_ok(body_string));
        let dog_image_url =
          Yojson.Basic.Util.(body_json |> member("message") |> to_string);

        let response_body =
          Format.asprintf(
            {|
       <!DOCTYPE html>
       <html lang="en">
         <head>
           <meta charset="utf-8" />
           <meta http-equiv="x-ua-compatible" content="ie=edge" />
           <meta name="viewport" content="width=device-width, initial-scale=1" />

           <title>OCaml isn't only for boring assignments</title>
         </head>

         <body>
           <h1>OCaml não serve só para exercicio chato de algoritmo de programação 1</h1>
           <img src="%s">
         </body>
       </html>
      |},
            dog_image_url,
          );

        Lwt.return_ok(Response.of_string(`OK, ~body=response_body));

      | Error(piaf_error) =>
        Lwt_result.fail(Piaf.Error.to_string(piaf_error))
      };
    };
  };

let setup_log = (~style_renderer=?, level) => {
  Fmt_tty.setup_std_outputs(~style_renderer?, ());
  Logs.set_level(level);
  Logs.set_reporter(Logs_fmt.reporter());
  ();
};

let () = {
  setup_log(Some(Logs.Debug));
  io_lambda(handleRequest);
};
