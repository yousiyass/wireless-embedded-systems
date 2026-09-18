import sqlite3
from fastapi import FastAPI, Request, Form
from fastapi.responses import RedirectResponse, PlainTextResponse
from fastapi.templating import Jinja2Templates
from starlette.responses import HTMLResponse

app = FastAPI()
templates = Jinja2Templates(directory=".")

conn = sqlite3.connect("project.db", check_same_thread=False)
cursor = conn.cursor()

cursor.execute("""
    CREATE TABLE IF NOT EXISTS machines (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    temperature INTEGER NOT NULL,
    fan_speed INTEGER NOT NULL
    )
""")
conn.commit()

cursor.execute("SELECT COUNT(*) FROM machines")
number_of_records = cursor.fetchone()[0]
if number_of_records == 0:
    cursor.execute("INSERT INTO machines (id, temperature, fan_speed) VALUES (1,20,1)",)
    cursor.execute("INSERT INTO machines (id, temperature, fan_speed) VALUES (2,45,2)",)
    cursor.execute("INSERT INTO machines (id, temperature, fan_speed) VALUES (3,70,3)")
    conn.commit()

@app.post("/machine_update")
def machine_updating(temp: int=Form(...), machine_id: int=Form(...)):
    print(f"--> INCOMING DATA: Makine ID = {machine_id} | Sicaklik = {temp}")
    if temp >= 20 and temp < 45:
        fan = 1
    elif temp >= 45 and temp < 70:
        fan = 2
    else:
        fan = 3

    cursor.execute("UPDATE machines SET temperature = ?, fan_speed = ? WHERE id = ?",(temp, fan, machine_id))
    conn.commit()

    cursor.execute("SELECT fan_speed FROM machines WHERE id = ?",(machine_id,))
    fan_speed = cursor.fetchone()[0]
    return PlainTextResponse(str(fan_speed))



@app.get("/home_page")
def home_page(request: Request):
    cursor.execute("SELECT temperature, fan_speed FROM machines WHERE id = 1")
    records1 = cursor.fetchall()


    cursor.execute("SELECT temperature, fan_speed FROM machines WHERE id = 2")
    records2 = cursor.fetchall()


    cursor.execute("SELECT temperature, fan_speed FROM machines WHERE id = 3")
    records3 = cursor.fetchall()


    return templates.TemplateResponse(
        request=request,
        name="final_project_main.html",
        context={
            "records1": records1,
            "records2": records2,
            "records3": records3,
        }
    )


