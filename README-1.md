# Movie Ticket Booking System

CSE 2102 – Data Structures, B.Tech Cyber Security.

Customers pick their **exact seats** on a seat map. Requests wait in a **queue** (first in, first out) and confirmed tickets sit on a **stack** (last in, first out). Every movie has its own screen with rows A and B, ten seats each.

## Files

- `index.html` – the website (open it in any browser)
- `movie_ticket.c` – the console version in C
- `README.md` – this file

## Put the website online with GitHub Pages

1. Go to github.com and create a new **public** repository (for example `movie-ticket-booking`).
2. Click **Add file → Upload files**, drag in `index.html`, `movie_ticket.c` and `README.md`, then click **Commit**.
3. Go to **Settings → Pages**, set the source to **Deploy from a branch**, choose `main` and `/ (root)`, and save.
4. After about a minute your link will be `https://YOUR-USERNAME.github.io/movie-ticket-booking/`.

The file must be named exactly `index.html`, or the link won't work.

## Run the C program

```
gcc movie_ticket.c -o movie_ticket
./movie_ticket
```

When you request a booking, enter the number of seats and then type each seat such as `A3` or `B7`.

## Team

- Mohamed Jaffar S – 250181601031
- Padmesh V – 250181601050
- Priyadharshini T – 250181601051
- Madhusudhan C – 250181601022

Assistant Professor In Charge: Archana, Department of Data Structure, B.S. Abdur Rahman Crescent Institute of Science & Technology.
