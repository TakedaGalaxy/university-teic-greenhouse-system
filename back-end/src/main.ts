import express from "express";
import morgan from "morgan";
import cors from "cors"
import 'dotenv/config'

const { PORT, ADDRESS } = process.env as any;

const app = express();

app.use(cors());

app.use(express.json());

app.use(morgan("dev"));

app.get("/", (rquest, response) => {
  response.status(200).end("Rodando !");
});

app.listen(Number(PORT), ADDRESS, () => {
  console.log(`[SERVER]: Running in http://${ADDRESS}:${PORT}`);
});