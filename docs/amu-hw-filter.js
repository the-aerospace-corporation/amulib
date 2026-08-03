class AmuHwFilter {

    static STORAGE_KEY = "amu-hw";

    // badge text -> hardware ids the command applies to (on) or not (off)
    static RULES = [
        { match: /AMU3 ESP32 only/i,         on: ["amu3esp32"] },
        { match: /Legacy only/i,             on: ["legacy"] },
        { match: /EYAS only/i,               on: ["eyas"] },
        { match: /Not implemented on AMU3/i, off: ["amu3arduino", "amu3esp32"] },
    ];

    static OPTIONS = [
        { id: "all",         label: "All hardware" },
        { id: "legacy",      label: "Legacy AMUs" },
        { id: "eyas",        label: "EYAS" },
        { id: "amu3arduino", label: "AMU3 (Arduino)" },
        { id: "amu3esp32",   label: "AMU3 ESP32" },
    ];

    static init() {
        document.addEventListener("DOMContentLoaded", () => {
            AmuHwFilter.injectSelect();
            AmuHwFilter.apply(AmuHwFilter.current());
        });
    }

    static current() {
        // ?hw=<id> is the stable, linkable interface - a bookmark or shared URL
        // always wins over whatever was last selected in this browser
        const fromQuery = new URLSearchParams(location.search).get("hw");
        if (AmuHwFilter.OPTIONS.some(o => o.id === fromQuery)) {
            localStorage.setItem(AmuHwFilter.STORAGE_KEY, fromQuery);
            return fromQuery;
        }
        const stored = localStorage.getItem(AmuHwFilter.STORAGE_KEY);
        // a stored id from an older taxonomy falls back to showing everything
        return AmuHwFilter.OPTIONS.some(o => o.id === stored) ? stored : "all";
    }

    // Reflect the selection in the URL (add/drop ?hw=) without pushing a new
    // history entry - the address bar becomes a shareable link to this view.
    static syncQuery(hw) {
        const url = new URL(location.href);
        if (hw === "all") {
            url.searchParams.delete("hw");
        } else {
            url.searchParams.set("hw", hw);
        }
        history.replaceState(null, "", url);
    }

    static injectSelect() {
        const host = document.getElementById("projectalign") ||
                     document.getElementById("titlearea");
        if (!host) return;

        const select = document.createElement("select");
        select.id = "amu-hw-select";
        select.title = "Show commands for this hardware";
        for (const opt of AmuHwFilter.OPTIONS) {
            const o = document.createElement("option");
            o.value = opt.id;
            o.textContent = opt.label;
            select.appendChild(o);
        }
        select.value = AmuHwFilter.current();
        select.addEventListener("change", () => {
            localStorage.setItem(AmuHwFilter.STORAGE_KEY, select.value);
            AmuHwFilter.syncQuery(select.value);
            AmuHwFilter.apply(select.value);
        });
        host.appendChild(select);
    }

    // A command "block" is either the field table row holding the enum value's
    // docs, or (on pages) the title plus its siblings up to the next title.
    static blocks() {
        const out = [];
        for (const title of document.querySelectorAll(".amucmdtitle")) {
            const row = title.closest("tr");
            if (row && row.querySelector(".fielddoc")) {
                out.push({ badge: row.querySelector(".hwbadge"), nodes: [row] });
                continue;
            }
            const nodes = [title];
            let el = title.nextElementSibling;
            while (el && !(el.classList && el.classList.contains("amucmdtitle"))) {
                nodes.push(el);
                el = el.nextElementSibling;
            }
            const badge = nodes.map(n => n.querySelector ? n.querySelector(".hwbadge") : null)
                               .find(b => b) || null;
            out.push({ badge, nodes });
        }
        return out;
    }

    static visible(badge, hw) {
        if (hw === "all" || !badge) return true;
        const text = badge.textContent;
        for (const rule of AmuHwFilter.RULES) {
            if (!rule.match.test(text)) continue;
            if (rule.on)  return rule.on.includes(hw);
            if (rule.off) return !rule.off.includes(hw);
        }
        return true;  // unknown badge text -> never hide silently
    }

    static apply(hw) {
        document.body.classList.toggle("amu-hw-filtered", hw !== "all");
        for (const block of AmuHwFilter.blocks()) {
            const show = AmuHwFilter.visible(block.badge, hw);
            for (const n of block.nodes) {
                n.classList.toggle("amu-hw-hidden", !show);
            }
        }
        AmuHwFilter.collapseEmptySections(hw);
    }

    // A section whose every command is filtered should go too - an empty shell
    // reads as broken rather than filtered.
    static collapseEmptySections(hw) {
        // Enum doc sections: hide the member block (and its title) when all of
        // its field rows are hidden
        for (const table of document.querySelectorAll("table.fieldtable")) {
            const titles = table.querySelectorAll(".amucmdtitle");
            if (!titles.length) continue;
            const empty = [...titles].every(t => {
                const row = t.closest("tr");
                return row && row.classList.contains("amu-hw-hidden");
            });
            const memitem = table.closest(".memitem");
            if (!memitem) continue;
            memitem.classList.toggle("amu-hw-hidden", empty);
            const title = memitem.previousElementSibling;
            if (title && title.classList.contains("memtitle")) {
                title.classList.toggle("amu-hw-hidden", empty);
            }
        }

        // Whole page: if every command on it is filtered, hide the content and
        // say why instead of leaving a stripped shell
        const contents = document.querySelector("div.contents");
        const notice = document.getElementById("amu-hw-notice");
        if (notice) notice.remove();
        if (!contents) return;

        const titles = contents.querySelectorAll(".amucmdtitle");
        const allHidden = titles.length > 0 && [...titles].every(t =>
            t.classList.contains("amu-hw-hidden") ||
            (t.closest("tr") && t.closest("tr").classList.contains("amu-hw-hidden")));

        contents.classList.toggle("amu-hw-hidden", allHidden);
        if (allHidden) {
            const label = (AmuHwFilter.OPTIONS.find(o => o.id === hw) || {}).label || hw;
            const div = document.createElement("div");
            div.id = "amu-hw-notice";
            div.textContent = "Nothing here exists on " + label +
                " - switch the hardware selector to “All hardware” to see these commands.";
            contents.parentNode.insertBefore(div, contents);
        }
    }
}
