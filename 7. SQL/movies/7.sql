SELECT title, rating from movies JOIN ratings ON id=movie_id WHERE year=2010 ORDER BY rating DESC, title;
